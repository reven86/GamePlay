#include "SocialController.h"
#include "social/ScoreloopSocialSession.h"
#include "social/GooglePlaySocialSession.h"
#include "social/GameCenterSocialSession.h"
#include "Base.h"
#include "Game.h"

namespace gameplay
{

SocialController::SocialController()
    : _session(NULL), _properties(NULL)
{
}

SocialController::~SocialController()
{
	SAFE_DELETE(_properties)
}

void SocialController::initialize()
{
}

void SocialController::finalize()
{
	if (_session)
		_session->synchronizeAchievements();
}

void SocialController::pause()
{
	if (_session)
		_session->synchronizeAchievements();
}

void SocialController::resume()
{
}

void SocialController::update(float elapsedTime)
{
}

bool SocialController::handleEvent(PlatformEvent *event)
{
	if (_session)
		return _session->handleEvent(event);

	return false;
}

void SocialController::loadConfig()
{
    if (_properties == NULL)
    {
        // Try to load custom config from file.
        if (FileSystem::fileExists("social.config"))
        {
            _properties = Properties::create("social.config");

            // Load filesystem aliases.
            Properties* aliases = _properties->getNamespace("aliases", true);
            if (aliases)
            {
                FileSystem::loadResourceAliases(aliases);
            }
        }
        else
        {
        	GP_ERROR("Failed to find the social.config file.");
        }
    }
}

void SocialController::authenticate(SocialSessionListener* listener)
{
#if defined(GP_USE_SOCIAL)
	if (!_properties)
		loadConfig();

	Properties *socialProperties = (_properties) ? _properties->getNamespace("social", true) : 0;
    const char* providerStr = "";

    if (socialProperties)
    {
    	providerStr = socialProperties->getString("provider");
    }

#if defined(__QNX__)
    if (strcmp(providerStr, "Scoreloop") == 0)
    {
        _session = ScoreloopSocialSession::authenticate(listener, socialProperties);
    }
    else
    {
        listener->authenticateEvent(SocialSessionListener::ERROR_INITIALIZATION, NULL);
    }
#elif defined(__ANDROID__)
    if (strcmp(providerStr, "GooglePlay") == 0)
    {
        _session = GooglePlaySocialSession::authenticate(listener, socialProperties);
    }
    else
    {
        listener->authenticateEvent(SocialSessionListener::ERROR_INITIALIZATION, NULL);
    }
#elif defined(__APPLE__) && (__MAC_OS_X_VERSION_MIN_REQUIRED > 1070 || __IPHONE_OS_VERSION_MIN_REQUIRED >= 60000)
    if (strcmp(providerStr, "GameCenter") == 0)
    {
        _session = GameCenterSocialSession::authenticate(listener, socialProperties);
    }
    else
    {
        listener->authenticateEvent(SocialSessionListener::ERROR_INITIALIZATION, NULL);
    }
#endif
#endif
}

}
