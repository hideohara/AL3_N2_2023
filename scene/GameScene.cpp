#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>

#include "AxisIndicator.h"

GameScene::GameScene() {}

// デストラクタ
GameScene::~GameScene() {

	// 自キャラの解放
	delete player_;
	delete model_; 
	delete debugCamera_;
	delete enemy_;
}

// 初期化
void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	// デバッグカメラの生成
	debugCamera_ = new DebugCamera(WinApp::kWindowWidth, WinApp::kWindowHeight);
	// 軸方向表示の表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	// 軸方向表示が参照するビュープロジェクションを指定する（アドレス渡し）
	AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection_);

	// ビュープロジェクション
	viewProjection_.Initialize();

	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("mario.jpg");

	// 3Dモデルの生成
	model_ = Model::Create();

	
	// 自キャラの生成
	player_ = new Player();
	// 自キャラの初期化
	player_->Initialize(model_, textureHandle_);

	// 敵キャラの生成
	enemy_ = new Enemy();
	Vector3 enemyPosition(3, 2.0f, 50.0f);
	enemy_->Initialize(model_, enemyPosition);

	
	// 敵キャラに自キャラのアドレスを渡す
	enemy_->SetPlayer(player_);
}

void GameScene::Update() {

#ifdef _DEBUG
	if (input_->TriggerKey(DIK_0)) {
		// フラグをトグル
		isDebugCameraActive_ = !isDebugCameraActive_;
	}
#endif
	if (isDebugCameraActive_) {
		debugCamera_->Update();
		viewProjection_.matView = debugCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
		// ビュープロジェクションの転送
		viewProjection_.TransferMatrix();
	} else {
		// ビュープロジェクション行列の更新と転送
		viewProjection_.UpdateMatrix();
	}


	// デバッグカメラの更新
	debugCamera_->Update();

	// 自キャラの更新
	player_->Update();
	// 敵の更新
	enemy_->Update();

	// 衝突判定
	CheckAllCollisions();
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>

	
	// 自キャラの描画
	player_->Draw(viewProjection_);
	// 敵の描画
	enemy_->Draw(viewProjection_);

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void GameScene::CheckAllCollisions() {

	// 判定対象AとBの座標
	Vector3 posA, posB;

	// 自弾リストの取得
	const std::list<PlayerBullet*>& playerBullets = player_->GetBullets();
	// 敵弾リストの取得
	const std::list<EnemyBullet*>& enemyBullets = enemy_->GetBullets();

#pragma region 自キャラと敵弾の当たり判定
	{
		// 自キャラの座標
		posA = player_->GetWorldPosition();

		// 自キャラと敵弾全ての当たり判定
		for (EnemyBullet* bullet : enemyBullets) {
			// 敵弾の座標
			posB = bullet->GetWorldPosition();
			// 座標の差分ベクトル
			Vector3 subtract = posB - posA;
			// 座標AとBの距離を求める
			float distance = Length(subtract);
			// 球と球の交差判定
			if (distance < 1.5f + 1.5f) {
				// 自キャラの衝突時コールバックを呼び出す
				player_->OnCollision();
				// 敵弾の衝突時コールバックを呼び出す
				bullet->OnCollision();
			}
		}
	}
#pragma endregion

#pragma region 自弾と敵キャラの当たり判定
	{
		// 敵キャラの座標
		posA = enemy_->GetWorldPosition();

		// 敵キャラと自弾全ての当たり判定
		for (PlayerBullet* bullet : playerBullets) {
			// 敵弾の座標
			posB = bullet->GetWorldPosition();
			// 座標の差分ベクトル
			Vector3 subtract = posB - posA;
			// 座標AとBの距離を求める
			float distance = Length(subtract);
			// 球と球の交差判定
			if (distance < 1.5f + 1.5f) {
				// 自弾の衝突時コールバックを呼び出す
				enemy_->OnCollision();
				// 敵弾の衝突時コールバックを呼び出す
				bullet->OnCollision();
			}
		}
	}
#pragma endregion

#pragma region 自弾と敵弾の当たり判定
	{
		// 自弾全てについて
		for (PlayerBullet* playerBullet : playerBullets) {
			// 自弾の座標
			posA = playerBullet->GetWorldPosition();

			// 敵弾全てについて
			for (EnemyBullet* enemyBullet : enemyBullets) {
				// 敵弾の座標
				posB = enemyBullet->GetWorldPosition();

				// 座標の差分ベクトル
				Vector3 subtract = posB - posA;
				// 座標AとBの距離を求める
				float distance = Length(subtract);
				// 球と球の交差判定
				if (distance < 1.5f + 1.5f) {
					// 自弾の衝突時コールバックを呼び出す
					playerBullet->OnCollision();
					// 敵弾の衝突時コールバックを呼び出す
					enemyBullet->OnCollision();
				}
			}
		}
	}
#pragma endregion
}
