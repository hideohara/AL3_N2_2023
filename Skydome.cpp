#include "Skydome.h"
#include "MathUtilityForText.h"
#include <cassert>

void Skydome::Initialize(Model* model) {
	// NULLポインタチェック
	assert(model);

	model_ = model;

	// ワールド変換の初期化
	worldTransform_.Initialize();
}

void Skydome::Update() {
	// 行列更新
	worldTransform_.matWorld_ = MakeAffineMatrix(
	    worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();
}

void Skydome::Draw(const ViewProjection& viewProjection) {
	model_->Draw(worldTransform_, viewProjection);
}
