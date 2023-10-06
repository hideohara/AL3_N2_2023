#include "Player.h"
#include <cassert>
#include "ImGuiManager.h"
#include "Vector3.h"

#include <algorithm>

void Player::Initialize(Model* model, uint32_t textureHandle) {

	// NULLポインタチェック
	assert(model);

	textureHandle_ = textureHandle;
	model_ = model;

	// ワールド変換の初期化
	worldTransform_.Initialize();

	
// シングルトンインスタンスを取得する
	input_ = Input::GetInstance();
}

void Player::Update() {

	// 移動
	move();


	// 行列更新
	worldTransform_.matWorld_ = MakeAffineMatrix(
	    worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);




	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();

	//// キャラクターの座標を画面表示する処理
	ImGui::Begin("Player");
	ImGui::SliderFloat3("Player", (float*)&worldTransform_.translation_, -100, 100);
	ImGui::End();
}


void Player::Draw(ViewProjection& viewProjection) {
	// 3Dモデルを描画
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
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
