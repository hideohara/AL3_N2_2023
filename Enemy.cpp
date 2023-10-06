#include "Enemy.h"
#include "ImGuiManager.h"
#include "MathUtilityForText.h"
#include <cassert>

void Enemy::Initialize(Model* model, const Vector3& position) {
	// NULLポインタチェック
	assert(model);

	model_ = model;
	// テクスチャ読み込み
	textureHandle_ = TextureManager::Load("enemy01.png");

	// ワールド変換の初期化
	worldTransform_.translation_ = position;
	worldTransform_.Initialize();
}

void Enemy::Update() {

	// 移動（ベクトルを加算）
	worldTransform_.translation_ += Vector3(0, 0, -0.1f);

	// 行列更新
	worldTransform_.matWorld_ = MakeAffineMatrix(
	    worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();

	// キャラクターの座標を画面表示する処理
	ImGui::Begin("Enemy");
	ImGui::SliderFloat3("Translation", (float*)&worldTransform_.translation_, -100, 100);
	ImGui::End();
}

void Enemy::Draw(const ViewProjection& viewProjection) {
	// モデルの描画
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
}
