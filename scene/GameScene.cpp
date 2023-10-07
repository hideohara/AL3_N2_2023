#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>

#include "AxisIndicator.h"

GameScene::GameScene() {}

// デストラクタ
GameScene::~GameScene() {
	// 弾
	for (EnemyBullet* bullet : enemyBullets_) {
		delete bullet;
	}
	// 敵
	for (Enemy* enemy : enemies_) {
		delete enemy;
	}

	// 自キャラの解放
	delete railCamera_;
	delete modelSkydome_;
	delete skydome_;
	delete debugCamera_;
	delete player_;
	delete model_; 
}

// 初期化
void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	// デバッグカメラの生成
	debugCamera_ = new DebugCamera(WinApp::kWindowWidth, WinApp::kWindowHeight);
	debugCamera_->SetFarZ(2000.0f);

	// 軸方向表示の表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	// 軸方向表示が参照するビュープロジェクションを指定する（アドレス渡し）
	AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection_);

	// ビュープロジェクション
	viewProjection_.farZ = 2000.0f;
	viewProjection_.Initialize();

	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("mario.jpg");

	// 3Dモデルの生成
	model_ = Model::Create();

	
	// 自キャラの生成
	player_ = new Player();
	// 自キャラの初期化
	Vector3 playerPosition(0, 0, 50.0f);
	player_->Initialize(model_, textureHandle_, playerPosition);



	// 3Dモデルの生成
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);
	// 天球の生成
	skydome_ = new Skydome();
	skydome_->Initialize(modelSkydome_);
	// レールカメラの生成
	railCamera_ = new RailCamera();
	railCamera_->Initialize(Vector3(0, 0, -50), Vector3(0, 0, 0));

	// 自キャラとレールカメラの親子関係を結ぶ
	player_->SetParent(&railCamera_->GetWorldTransform());

	PopEnemy({3, 0, 50});
	PopEnemy({10, 0, 50});
}

void GameScene::Update() {
	// レールカメラの更新
	railCamera_->Update();

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

		viewProjection_.matView = railCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = railCamera_->GetViewProjection().matProjection;
		// ビュープロジェクションの転送
		viewProjection_.TransferMatrix();
	}


	// デバッグカメラの更新
	debugCamera_->Update();

	// 自キャラの更新
	player_->Update();
	// 天球の更新
	skydome_->Update();
	// 衝突判定
	CheckAllCollisions();

	// 弾更新
	for (EnemyBullet* bullet : enemyBullets_) {
		bullet->Update();
	}
	// 敵の更新
	for (auto& enemy : enemies_) {
		enemy->Update();
	}

	// デスフラグの立った弾を削除
	enemyBullets_.remove_if([](EnemyBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});
	// デスフラグの立った敵を削除
	enemies_.remove_if([](Enemy* enemy) {
		if (enemy->IsDead()) {
			delete enemy;
			return true;
		}
		return false;
	});
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

	// 天球の描画
	skydome_->Draw(viewProjection_);
	// 自キャラの描画
	player_->Draw(viewProjection_);

	// 敵の描画
	for (auto& enemy : enemies_) {
		enemy->Draw(viewProjection_);
	}
	// 弾描画
	for (EnemyBullet* bullet : enemyBullets_) {
		bullet->Draw(viewProjection_);
	}

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
	//const std::list<EnemyBullet*>& enemyBullets = enemy_->GetBullets();
	// 敵弾リストの取得
	const std::list<EnemyBullet*>& enemyBullets = enemyBullets_;

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
		// 敵全てについて
		for (Enemy* enemy : enemies_) {
			// 敵キャラの座標
			posA = enemy->GetWorldPosition();

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
					enemy->OnCollision();
					// 敵弾の衝突時コールバックを呼び出す
					bullet->OnCollision();
				}
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


void GameScene::AddEnemyBullet(EnemyBullet* enemyBullet) {
	// リストに登録する
	enemyBullets_.push_back(enemyBullet);
}

// 敵の発生
void GameScene::PopEnemy(const Vector3& position) {

	// 敵キャラの生成
	Enemy* enemy = new Enemy();
	// リストに登録する
	enemies_.push_back(enemy);

	//Vector3 enemyPosition(3, 2.0f, 50.0f);
	enemy->Initialize(model_, position);
	// 敵キャラに自キャラのアドレスを渡す
	enemy->SetPlayer(player_);
	// 敵キャラにゲームシーンを渡す
	enemy->SetGameScene(this);
}
