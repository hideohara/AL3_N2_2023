#pragma once

#include "Model.h"
#include "WorldTransform.h"
#include "Input.h"
#include "MathUtilityforText.h"
#include "PlayerBullet.h"


#include <list>

class Player {

public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	~Player();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model">モデル</param>
	/// <param name="textureHandle">テクスチャハンドル</param>
	void Initialize(Model* model, uint32_t textureHandle, const Vector3& position);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

    /// <summary>
	/// 描画
	/// </summary>
	/// <param name="viewProjection">ビュープロジェクション（参照渡し）</param>
	void Draw(ViewProjection& viewProjection);

	
	/// <summary>
	/// 移動
	/// </summary>
	void move();

	/// <summary>
	/// 旋回（回転）
	/// </summary>
	void Rotate();
	
	/// <summary>
	/// 攻撃
	/// </summary>
	void Attack();

	
	// ワールド座標を取得
	Vector3 GetWorldPosition();
		
	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision();

	// 弾リストを取得
    const std::list<PlayerBullet*>& GetBullets() const { return bullets_; }

	/// <summary>
	/// 親となるワールドトランスフォームをセット
	/// </summary>
	/// <param name="parent">親となるワールドトランスフォーム</param>
	void SetParent(const WorldTransform* parent);


 private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;

	// キーボード入力
	Input* input_ = nullptr;

	
	// 弾
	//PlayerBullet* bullet_ = nullptr;
	std::list<PlayerBullet*> bullets_;


};
