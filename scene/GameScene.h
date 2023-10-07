#pragma once

#include "Audio.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "Player.h"
#include "DebugCamera.h"
#include "Enemy.h"
#include "Skydome.h"
#include "RailCamera.h"

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	
    /// <summary>
	/// 衝突判定と応答
	/// </summary>
	void CheckAllCollisions();

	// 弾リストを取得
	const std::list<EnemyBullet*>& GetBullets() { return enemyBullets_; }

	
    /// <summary>
	/// 敵弾を追加する
	/// </summary>
	/// <param name="enemyBullet">敵弾</param>
	void AddEnemyBullet(EnemyBullet* enemyBullet);

	/// <summary>
	/// 敵の発生
	/// </summary>
	void PopEnemy(const Vector3& position);

	
    /// <summary>
	/// 敵発生データの読み込み
	/// </summary>
	void LoadEnemyPopData();

	
    /// <summary>
	/// 敵発生コマンドの更新
	/// </summary>
	void UpdateEnemyPopCommands();

	bool GetIsEnd() { return isEnd_; }

	void Start();

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	/// <summary>
	/// ゲームシーン用
	/// </summary>
	// ビュープロジェクション
	ViewProjection viewProjection_;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;
	// モデルデータ
	Model* model_ = nullptr;

	// 自キャラ
	Player* player_ = nullptr;

	
    // デバッグカメラ有効
	bool isDebugCameraActive_ = false;
	// デバッグカメラ
	DebugCamera* debugCamera_ = nullptr;

	// 天球
	Skydome* skydome_ = nullptr;
	// 3Dモデル
	Model* modelSkydome_ = nullptr;
	// レールカメラ
	RailCamera* railCamera_ = nullptr;
	// 弾
	std::list<EnemyBullet*> enemyBullets_;
	// 敵
	std::list<Enemy*> enemies_;
	
    //  敵発生コマンド
	std::stringstream enemyPopCommands;
	bool isWait = false;
	int32_t counter;
	
	// 終了フラグ
	bool isEnd_ = false;
};
