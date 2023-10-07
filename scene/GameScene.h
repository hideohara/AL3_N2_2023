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

	// 敵
	Enemy* enemy_ = nullptr;
	// 天球
	Skydome* skydome_ = nullptr;
	// 3Dモデル
	Model* modelSkydome_ = nullptr;
};
