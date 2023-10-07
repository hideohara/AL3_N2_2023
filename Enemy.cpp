#include "Enemy.h"
#include "ImGuiManager.h"
#include "MathUtilityForText.h"
#include <cassert>
#include "Player.h"
#include "GameScene.h"
#include <sstream>

Enemy::~Enemy() {

}

void Enemy::Initialize(Model* model, const Vector3& position) {
	// NULLポインタチェック
	assert(model);

	model_ = model;
	// テクスチャ読み込み
	textureHandle_ = TextureManager::Load("enemy01.png");

	// ワールド変換の初期化
	worldTransform_.translation_ = position;
	worldTransform_.Initialize();

	// 接近フェーズ初期化
	PhaseApproachInitialize();
}

void Enemy::Update() {
	switch (phase_) {
	case Phase::Approach:
	default:
		PhaseApproach();
		break;
	case Phase::Leave:
		PhaseLeave();
		break;
	}

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


void Enemy::PhaseApproach() {
	ImGui::Begin("Enemy");
	ImGui::Text("Phase: Approach");
	ImGui::End();

	// 移動（ベクトルを加算）
	worldTransform_.translation_ += Vector3(0, 0, -0.1f);
	// 規定の位置に到達したら離脱
	if (worldTransform_.translation_.z < 0.0f) {
		phase_ = Phase::Leave;
	}

	if (--fireTimer <= 0) {
		// 弾を発射
		Fire();
		// 発射タイマーを初期化
		fireTimer = kFireInterval;
	}
}

void Enemy::PhaseLeave() {
	ImGui::Begin("Enemy");
	ImGui::Text("Phase: Leave");
	ImGui::End();

	// 移動（ベクトルを加算）
	worldTransform_.translation_ += Vector3(-0.1f, 0.1f, -0.1f);
}

void Enemy::Fire() {
	// 弾の速度
	const float kBulletSpeed = 1.0f;
	//Vector3 velocity(0, 0, -kBulletSpeed);

	// 速度ベクトルを自機の向きに合わせて回転させる
	//velocity = TransformNormal(velocity, worldTransform_.matWorld_);

	
	// 自キャラのワールド座標を取得する
	Vector3 targetPos = player_->GetWorldPosition();
	// 敵キャラのワールド座標を取得する
	Vector3 basePos = this->GetWorldPosition();
	// 敵キャラ→自キャラの差分ベクトル
	Vector3 velocity = targetPos - basePos;
	// ベクトルの正規化
	velocity = Normalize(velocity);
	// ベクトルの長さを、早さに合わせる
	velocity *= kBulletSpeed;

	// 弾を生成し、初期化
	EnemyBullet* newBullet = new EnemyBullet();
	newBullet->Initialize(model_, worldTransform_.translation_, velocity);

	// 弾を登録する
	gameScene_->AddEnemyBullet(newBullet);
}

void Enemy::PhaseApproachInitialize() {
	// 発射タイマーを初期化

	fireTimer = kFireInterval;
}

Vector3 Enemy::GetWorldPosition() {
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];
	return worldPos;
}

void Enemy::OnCollision() {
	// デスフラグを立てる
	isDead_ = true;
}