#ifndef LOADINGSTATE_HPP
#define LOADINGSTATE_HPP

#include "GameState.hpp"
#include "Logger.hpp"

/**
 * State that the Game is in when everything is being loaded.
 */
class LoadingState : public GameState, public Logger::Listener
{
public:
	void onLog( const std::string& text );
	void update();

private:
	int lastRenderTime;
};

#endif // LOADINGSTATE_HPP
