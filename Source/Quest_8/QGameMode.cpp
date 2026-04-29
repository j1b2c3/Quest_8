#include "QGameMode.h"
#include "QPlayerController.h"
#include "QCharacter.h"
#include "QGameState.h"

AQGameMode::AQGameMode()
{
	PlayerControllerClass = AQPlayerController::StaticClass();
	DefaultPawnClass = AQCharacter::StaticClass();
	GameStateClass = AQGameState::StaticClass();
}
