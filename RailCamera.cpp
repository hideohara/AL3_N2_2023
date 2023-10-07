#include "RailCamera.h"
#include "ImGuiManager.h"
#include "MathUtilityForText.h"

void RailCamera::Initialize(const Vector3& position, const Vector3& rotation) {
	// ワールドトランスフォームの初期化
	worldTransform_.translation_ = position;
	worldTransform_.rotation_ = rotation;
	worldTransform_.Initialize();
	// ビュープロジェクションの初期化
	viewProjection_.farZ = 2000.0f;
	viewProjection_.Initialize();
}

void RailCamera::Update() {
	// 移動（ベクトルを加算）
	worldTransform_.translation_ += Vector3(0, 0, 0.1f);
	// ワールド行列の更新
	worldTransform_.matWorld_ = MakeAffineMatrix(
	    worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

	// カメラオブジェクトのワールド行列からビュー行列を計算する
	viewProjection_.matView = Inverse(worldTransform_.matWorld_);

	// ビュープロジェクションを転送
	viewProjection_.TransferMatrix();

	// カメラの座標を画面表示する処理
	ImGui::Begin("Camera");
	ImGui::SliderFloat3("Translation", (float*)&worldTransform_.translation_, -100, 100);
	ImGui::SliderFloat3("Rotation", (float*)&worldTransform_.rotation_, -PI, PI);
	ImGui::End();
}