#include "GameOver.h"

#include "TextureManager.h"
#include <cassert>

// デストラクタ
GameOver::~GameOver() {
	delete sprite_;
	delete spriteEnter_;
}

// 初期化
void GameOver::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("GameOver.png");
	// スプライトの生成
	sprite_ = Sprite::Create(textureHandle_, {0, 100});
	// ファイル名を指定してテクスチャを読み込む
	textureHandleEnter_ = TextureManager::Load("enter.png");
	// スプライトの生成
	spriteEnter_ = Sprite::Create(textureHandleEnter_, {400, 500});
}

void GameOver::Update() {
	if (input_->TriggerKey(DIK_RETURN)) {
		isEnd_ = true;
	}
}

void GameOver::Draw() {

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

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	sprite_->Draw();
	spriteEnter_->Draw();

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void GameOver::Start() { isEnd_ = false; }
