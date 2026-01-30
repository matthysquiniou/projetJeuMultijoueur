#include "Controller.h"
#include "WrapperInput.h"
#include "UIWrapper.h"
#include "Player.h"
#include "Camera.h"
#include "Util.h"
#include "App.h"

void Controller::Update(SceneManager& sceneManager)
{
	GameState gameState = App::GetInstance().GetGameState();
	switch (gameState)
	{
		case GameState::CONNECT:
			break;
		case GameState::NAME:
			break;
		case GameState::PLAY:
		{
			MultiEntity* playerME = sceneManager.GetFirstMultiEntityOfTag(EntityTag::ACTIVE_PLAYER);
			float dt = cpuTime.delta;
			if (playerME != nullptr)
			{
				Player* player = dynamic_cast<Player*>(playerME);

				if (WrapperInput::IsKeyDown(ControllerBind::PLAYER_FORWARD))
					player->EngineGoForward();
				if (WrapperInput::IsKeyDown(ControllerBind::PLAYER_BACK))
					player->EngineGoBack();
				if (WrapperInput::IsKeyDown(ControllerBind::PLAYER_LEFT))
					player->HelmGoLeft();
				if (WrapperInput::IsKeyDown(ControllerBind::PLAYER_RIGHT))
					player->HelmGoRight();
				if (WrapperInput::IsKeyDown(ControllerBind::PLAYER_SHOOT))
					player->Shoot();
				if (WrapperInput::IsKeyDown(ControllerBind::PLAYER_FOLLOW_LAST_SHOOT))
					Camera::SwitchCameraMode(CameraMode::FOLLOW_LAST_BULLET);
				if (WrapperInput::IsKeyUp(ControllerBind::PLAYER_FOLLOW_LAST_SHOOT))
					Camera::SwitchCameraMode(CameraMode::FOLLOW_PLAYER);
			}


			DirectX::XMFLOAT2 cursorDelta = Util::GetCursorDelta();
			Util::SetCursorPositionCenter();
			Util::ResetCursorDelta();
			Camera::UpdateRotation(cursorDelta);
			break;
		}
	}
}
