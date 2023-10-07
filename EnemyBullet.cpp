#include "EnemyBullet.h"

#include <cassert>

void EnemyBullet::Initialize(Model* model, const Vector3& position, const Vector3& velocity) {

	// NULLポインタチェック
	assert(model);

	// 引数で受け取った速度をメンバ変数に代入
	model_ = model;
	// テクスチャ読み込み
	textureHandle_ = TextureManager::Load("red.png");
	// 引数で受け取った速度をメンバ変数に代入
	velocity_ = velocity;

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	// 引数で受け取った初期座標をセット
	worldTransform_.translation_ = position;
}

void EnemyBullet::Update() {
	// 座標を移動させる（1フレーム分の移動量を足しこむ）
	worldTransform_.translation_ += velocity_;

	// 時間経過でデス
	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}

	// 行列更新
	worldTransform_.matWorld_ = MakeAffineMatrix(
	    worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();
}

void EnemyBullet::Draw(const ViewProjection& viewProjection) {
	// モデルの描画
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
}

void EnemyBullet::OnCollision() {
	// デスフラグを立てる
	isDead_ = true;
}

Vector3 EnemyBullet::GetWorldPosition() {
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];
	return worldPos;
}
