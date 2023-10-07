#include "Player.h"
#include <cassert>
#include "ImGuiManager.h"
#include "Vector3.h"
#include "DirectXCommon.h"
#include <algorithm>

Player::~Player() {
	//// 弾更新
	for (PlayerBullet* bullet : bullets_) {
		delete bullet;
	}
	delete sprite2DReticle_;
}

void Player::Initialize(Model* model, uint32_t textureHandle, const Vector3& position) {

	// NULLポインタチェック
	assert(model);

	textureHandle_ = textureHandle;
	model_ = model;

	// ワールド変換の初期化
	worldTransform_.translation_ = position;
	worldTransform_.Initialize();

	// シングルトンインスタンスを取得する
	input_ = Input::GetInstance();
	
	// 3Dレティクルのワールドトランスフォーム初期化
	worldTransform3DReticle_.Initialize();

	// レティクル用テクスチャ取得
	uint32_t textureReticle = TextureManager::Load("reticle.png");

	// スプライト生成
	//sprite2DReticle_ = Sprite::Create(textureReticle, 座標, 色, アンカーポイント);
	sprite2DReticle_ = Sprite::Create(
	    textureReticle, Vector2(WinApp::kWindowWidth / 2.0f, WinApp::kWindowHeight / 2.0f),
	    Vector4(1, 1, 1, 1), Vector2(0.5f, 0.5f));
}

void Player::Update(const ViewProjection& viewProjection) {
	// デスフラグの立った弾を削除
	bullets_.remove_if([](PlayerBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});

	// 移動
	move();

	// 旋回（回転）
	Rotate();

	// 攻撃
	Attack();

	// 行列更新
	worldTransform_.matWorld_ = MakeAffineMatrix(
	    worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

	// 親があれば親のワールド行列を掛ける
	if (worldTransform_.parent_) {
		worldTransform_.matWorld_ *= worldTransform_.parent_->matWorld_;
	}

	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();

	//// キャラクターの座標を画面表示する処理
	ImGui::Begin("Player");
	ImGui::SliderFloat3("Player", (float*)&worldTransform_.translation_, -100, 100);
	ImGui::End();

	// 弾更新
	for (PlayerBullet* bullet : bullets_) {
		bullet->Update();
	}

	// 自機のワールド座標から3Dレティクルのワールド座標を計算
	{
		// 自機から3Dレティクルへの距離
		const float kDistancePlayerTo3DReticle = 50.0f;
		// 自機から3Dレティクルへのオフセット(Z+向き)
		Vector3 offset = {0, 0, 1.0f};
		// 自機のワールド行列の回転を反映
		offset = TransformNormal(offset, worldTransform_.matWorld_);
		// ベクトルの長さを整える
		offset = Normalize(offset) * kDistancePlayerTo3DReticle;
		// 3Dレティクルの座標を設定
		worldTransform3DReticle_.translation_ = GetWorldPosition() + offset;

		// 行列更新
		worldTransform3DReticle_.matWorld_ = MakeAffineMatrix(
		    worldTransform3DReticle_.scale_, worldTransform3DReticle_.rotation_,
		    worldTransform3DReticle_.translation_);

		// 行列を定数バッファに転送
		worldTransform3DReticle_.TransferMatrix();
	}

	// 3Dレティクルのワールド座標から2Dレティクルのスクリーン座標を計算
	{
		// 3Dレティクルのワールド行列から、ワールド座標を取得;
		Vector3 positionReticle = GetWorldPosition2DReticle();

		// ビューポート行列
		Matrix4x4 matViewport =
		    MakeViewportMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);

		// ビュー行列とプロジェクション行列、ビューポート行列を合成する
		Matrix4x4 matViewProjectionViewport =
		    viewProjection.matView * viewProjection.matProjection * matViewport;

		// ワールド→スクリーン座標変換（ここで3Dから2Dになる）
		positionReticle = Transform(positionReticle, matViewProjectionViewport);

		// スプライトのレティクルに座標設定
		sprite2DReticle_->SetPosition(Vector2(positionReticle.x, positionReticle.y));
	}

	{
		POINT mousePosition;
		// マウス座標(スクリーン座標)を取得する
		GetCursorPos(&mousePosition);

		// クライアントエリア座標に変換する
		HWND hwnd = WinApp::GetInstance()->GetHwnd();
		ScreenToClient(hwnd, &mousePosition);

		// マウス座標を2Dレティクルのスプライトに代入する
		// スプライトのレティクルに座標設定
		sprite2DReticle_->SetPosition(Vector2((float)mousePosition.x, (float)mousePosition.y));

		// ビュープロジェクションビューポート合成行列
		// ビューポート逆行列
		Matrix4x4 matInvViewport = MakeIdentityMatrix();
		matInvViewport.m[0][0] = +2.0f / WinApp::kWindowWidth;
		matInvViewport.m[1][1] = -2.0f / WinApp::kWindowHeight;
		matInvViewport.m[3][0] = -1;
		matInvViewport.m[3][1] = 1;

		// プロジェクション逆行列
		Matrix4x4 matInvProjection = Inverse(viewProjection.matProjection);
		// ビュー逆行列
		Matrix4x4 matInvView = Inverse(viewProjection.matView);
		// 逆行列の合成行列
		Matrix4x4 matInverse = matInvViewport * matInvProjection * matInvView;


		// スクリーン座標
		Vector2 spritePosition = sprite2DReticle_->GetPosition();

		Vector3 posNear = Vector3((float)spritePosition.x, (float)spritePosition.y, 0);
		Vector3 posFar = Vector3((float)spritePosition.x, (float)spritePosition.y, 1);

		// スクリーン座標系からワールド座標系へ
		posNear = Transform(posNear, matInverse);
		posFar = Transform(posFar, matInverse);

		// マウスレイの方向
		Vector3 mouseDirection = posFar - posNear;
		mouseDirection = Normalize(mouseDirection);
		// カメラから照準オブジェクトの距離
		const float kDistanceTestObject = 100.0f;
		// posNear から mouseDirection の方向に kDistanceTestObject 進んだ座標;
		worldTransform3DReticle_.translation_ = posNear + mouseDirection * kDistanceTestObject;
		
		//worldTransform3DReticle_のワールド行列更新と転送; 
		//  行列更新
		worldTransform3DReticle_.matWorld_ = MakeAffineMatrix(
		    worldTransform3DReticle_.scale_, worldTransform3DReticle_.rotation_,
		    worldTransform3DReticle_.translation_);

		// 行列を定数バッファに転送
		worldTransform3DReticle_.TransferMatrix();

		ImGui::Begin("Player2");
		ImGui::Text("2DReticle:(%f,%f)", spritePosition.x, spritePosition.y);
		ImGui::Text("Near:(%+.2f,%+.2f,%+.2f)", posNear.x, posNear.y, posNear.z);
		ImGui::Text("Far:(%+.2f,%+.2f,%+.2f)", posFar.x, posFar.y, posFar.z);
		ImGui::Text(
		    "3DReticle:(%+.2f,%+.2f,%+.2f)", worldTransform3DReticle_.translation_.x,
		    worldTransform3DReticle_.translation_.y, worldTransform3DReticle_.translation_.z);
		ImGui::End();

	}
}

// 描画
void Player::Draw(ViewProjection& viewProjection) {
	// 3Dモデルを描画
	model_->Draw(worldTransform_, viewProjection, textureHandle_);

	// 弾描画
	for (PlayerBullet* bullet : bullets_) {
		bullet->Draw(viewProjection);
	}

	// 3Dレティクルを描画
	model_->Draw(worldTransform3DReticle_, viewProjection, textureHandle_);
}

// 移動
void Player::move() {
	// キャラクターの移動ベクトル
	Vector3 move = {0, 0, 0};

	// キャラクターの移動速さ
	const float kCharacterSpeed = 0.2f;

	// 押した方向で移動ベクトルを変更（左右）
	if (input_->PushKey(DIK_LEFT)) {
		move.x -= kCharacterSpeed;
	} else if (input_->PushKey(DIK_RIGHT)) {
		move.x += kCharacterSpeed;
	}

	// 押した方向で移動ベクトルを変更（上下）
	if (input_->PushKey(DIK_DOWN)) {
		move.y -= kCharacterSpeed;
	} else if (input_->PushKey(DIK_UP)) {
		move.y += kCharacterSpeed;
	}

	// 座標移動（ベクトルの加算）
	worldTransform_.translation_ += move;

	// 移動限界座標
	const float kMoveLimitX = 34.0f;
	const float kMoveLimitY = 18.0f;

	// 範囲を超えない処理
	worldTransform_.translation_.x =
	    std::clamp(worldTransform_.translation_.x, -kMoveLimitX, kMoveLimitX);
	worldTransform_.translation_.y =
	    std::clamp(worldTransform_.translation_.y, -kMoveLimitY, kMoveLimitY);
}

// 旋回（回転）
void Player::Rotate() {
	// 回転速さ[ラジアン/frame]
	const float kRotSpeed = 0.02f;

	// 押した方向で移動ベクトルを変更
	if (input_->PushKey(DIK_A)) {
		worldTransform_.rotation_.y -= kRotSpeed;
	} else if (input_->PushKey(DIK_D)) {
		worldTransform_.rotation_.y += kRotSpeed;
	}
}

// 攻撃
void Player::Attack() {

	if (input_->TriggerKey(DIK_SPACE)) {

		const float kBulletSpeed = 1.0f;
		Vector3 velocity(0, 0, kBulletSpeed);
		// 速度ベクトルを自機の向きに合わせて回転させる
		velocity = TransformNormal(velocity, worldTransform_.matWorld_);

		// 自キャラから照準オブジェクトへのベクトル
		velocity = worldTransform3DReticle_.translation_ - GetWorldPosition();
		velocity = Normalize(velocity) * kBulletSpeed;

		// 弾を生成し、初期化
		PlayerBullet* newBullet = new PlayerBullet();
		newBullet->Initialize(model_, GetWorldPosition(), velocity);

		// 弾を登録する
		bullets_.push_back(newBullet);
	}
}

Vector3 Player::GetWorldPosition() {
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];
	return worldPos;
}

void Player::OnCollision() {

}

void Player::SetParent(const WorldTransform* parent) {

	// 親子関係を結ぶ
	worldTransform_.parent_ = parent;
}

void Player::DrawUI() {
	// 2Dレティクルを描画
	sprite2DReticle_->Draw();
}

Vector3 Player::GetWorldPosition2DReticle() {
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransform3DReticle_.matWorld_.m[3][0];
	worldPos.y = worldTransform3DReticle_.matWorld_.m[3][1];
	worldPos.z = worldTransform3DReticle_.matWorld_.m[3][2];
	return worldPos;
}

void Player::Start() {
	// 弾を削除
	bullets_.remove_if([](PlayerBullet* bullet) {
			delete bullet;
			return true;
	});
}



