#if defined(__ANDROID__) && defined(GP_USE_SOCIAL)

#include "Base.h"
#include "GooglePlaySocialSession.h"
#include <android/log.h>
#include <sys/atomics.h>
#include "GooglePlaySocialJNI.h"

// Externally referenced global variable.
extern struct android_app* __state;

#define SIGNIN_FAILED 0
#define SIGNIN_IN_PROGRESS 1
#define SIGNIN_SUCCEEDED 2
static int _signInStatus = SIGNIN_FAILED;

JNIEXPORT void JNICALL Java_org_gameplay3d_sample_1spaceship_GooglePlaySocial_native_1ReportSignInState(JNIEnv *env, jobject thiz, jboolean signedIn, jboolean inProgress)
{
    int newState = inProgress ? SIGNIN_IN_PROGRESS : signedIn ? SIGNIN_SUCCEEDED : SIGNIN_FAILED;
    __atomic_swap(newState, &_signInStatus);

    if (signedIn && !inProgress)
    {
    	gameplay::GooglePlaySocialSession::finishedSignIn();
    }
}

JNIEXPORT void JNICALL Java_org_gameplay3d_sample_1spaceship_GooglePlaySocial_native_1ReportUserEntry(JNIEnv *env, jobject thiz, jstring name, jstring id)
{
	const char *nameStr = env->GetStringUTFChars(name, 0);
	const char *idStr = env->GetStringUTFChars(id, 0);
	gameplay::GooglePlaySocialSession::setUser(nameStr, idStr);
	env->ReleaseStringUTFChars(name, nameStr);
	env->ReleaseStringUTFChars(id, idStr);
}

JNIEXPORT void JNICALL Java_org_gameplay3d_sample_1spaceship_GooglePlaySocial_native_1ReportAchievementEntry(JNIEnv *env, jobject thiz, jint errorStatus, jstring name, jstring title, jstring dateAchieved, jint value, jint total)
{
	if (errorStatus != 0)
	{
		gameplay::GooglePlaySocialSession::handleAchievementError(errorStatus);
	}
	else if (value == -1 && total == -1 && title == NULL && name == NULL)
	{
		gameplay::GooglePlaySocialSession::finishedAddingAchievements();
	}
	else
	{
		const char *achievementName = env->GetStringUTFChars(name, 0);
		const char *achievementTitle = env->GetStringUTFChars(title, 0);
		const char *achievementDate = env->GetStringUTFChars(dateAchieved, 0);
		gameplay::GooglePlaySocialSession::addAchievement(achievementName, achievementTitle, value, total, achievementDate);
		env->ReleaseStringUTFChars(name, achievementName);
		env->ReleaseStringUTFChars(title, achievementTitle);
		env->ReleaseStringUTFChars(dateAchieved, achievementDate);
	}
}

JNIEXPORT void JNICALL Java_org_gameplay3d_sample_1spaceship_GooglePlaySocial_native_1ReportLeaderboardEntry(JNIEnv *env, jobject thiz, jint errorStatus, jint index, jstring playerId, jstring playerName, jint score)
{
	if (errorStatus != 0)
	{
		gameplay::GooglePlaySocialSession::handleScoresError(errorStatus);
	}
	else if (score == -1 && playerName == NULL && playerId == NULL)
	{
		gameplay::GooglePlaySocialSession::finishedAddingScores();
	}
	else
	{
		const char *playerStr = env->GetStringUTFChars(playerName, 0);
		gameplay::GooglePlaySocialSession::addScore(index + 1, playerStr, score);
		env->ReleaseStringUTFChars(playerName, playerStr);
	}
}

JNIEXPORT void JNICALL Java_org_gameplay3d_sample_1spaceship_GooglePlaySocial_native_1ReportFriendEntry(JNIEnv *env, jobject thiz, jint errorStatus, jstring playerName, jstring playerId)
{
	if (errorStatus != 0)
	{
		gameplay::GooglePlaySocialSession::handleFriendsError(errorStatus);
	}
	else if (playerName == NULL && playerId == NULL)
	{
		gameplay::GooglePlaySocialSession::finishedAddingFriends();
	}
	else
	{
		const char *playerStr = env->GetStringUTFChars(playerName, 0);
		gameplay::GooglePlaySocialSession::addFriend(playerStr);
		env->ReleaseStringUTFChars(playerName, playerStr);
	}
}

JNIEXPORT void JNICALL Java_org_gameplay3d_sample_1spaceship_GooglePlaySocial_native_1ReportCloudLoadResult(JNIEnv *env, jobject thiz, jint errorStatus, jstring data)
{
	if (errorStatus != 0)
	{
		gameplay::GooglePlaySocialSession::handleLoadDataError(errorStatus);
	}
	else
	{
		const char *dataStr = env->GetStringUTFChars(data, 0);
		gameplay::GooglePlaySocialSession::finishedLoadingData(dataStr);
		env->ReleaseStringUTFChars(data, dataStr);
	}
}

namespace gameplay
{

GooglePlaySocialSession* GooglePlaySocialSession::_session = NULL;

GooglePlaySocialSession::GooglePlaySocialSession()
    : SocialSession(),
      _listener(NULL), _properties(NULL), _pendingUserResponse(false), _pendingFriendsResponse(false),
      _pendingScoresResponse(false), _pendingSubmitScoreResponse(false), _pendingAchievementResponse(false),
      _pendingDataResponse(false),
      _key(NULL)
{
    _userOp = USEROP_GET_LOCALUSER;
}

GooglePlaySocialSession::~GooglePlaySocialSession()
{
}

SocialSessionListener* GooglePlaySocialSession::getListener()
{
    return _listener;
}

void GooglePlaySocialSession::getJNI(JNIEnv *&javaEnv, jclass &javaClass) const
{
	GP_ASSERT(__state && __state->activity && __state->activity->vm);

	JavaVM *jvm = __state->activity->vm;
    javaEnv = NULL;
    jvm->GetEnv((void **)&javaEnv, JNI_VERSION_1_6);
    jint r = jvm->AttachCurrentThread(&javaEnv, NULL);
    if (r == JNI_ERR)
        GP_ERROR("Failed to retrieve JVM environment to authenticate.");

    GP_ASSERT(javaEnv);
    javaClass = javaEnv->GetObjectClass(__state->activity->clazz);
}

void GooglePlaySocialSession::detachJNI() const
{
//	__state->activity->vm->DetachCurrentThread();
}

SocialSession *GooglePlaySocialSession::authenticate(SocialSessionListener* listener, Properties* properties)
{
	if (!_session)
    {
		_session = new GooglePlaySocialSession();
		_session->_listener = listener;
		_session->_properties = properties;

		JNIEnv *env;
		jclass clazz;
		_session->getJNI(env, clazz);

		if (env)
		{
			jmethodID signInMethod = env->GetMethodID(clazz, "gameServicesSignIn", "()V");
			if (signInMethod)
			{
				env->CallVoidMethod(__state->activity->clazz, signInMethod);
			}
			else
			{
				_session->_listener->authenticateEvent(SocialSessionListener::ERROR_INITIALIZATION, NULL);
				GP_ERROR("couldn't find gameServicesSignIN");
			}
		}
		else
		{
			_session->_listener->authenticateEvent(SocialSessionListener::ERROR_INITIALIZATION, NULL);
		}

		_session->detachJNI();
    }

    return _session;
}

void GooglePlaySocialSession::finishedSignIn()
{
    _session->_listener->authenticateEvent(SocialSessionListener::SUCCESS, _session);
}

void GooglePlaySocialSession::setUser(const char *playerName, const char *playerId)
{
    _session->_user.handle = (unsigned int)playerId;
    _session->_user.name = playerName;
}

const SocialPlayer& GooglePlaySocialSession::getUser() const
{
	return _user;
}

void GooglePlaySocialSession::addFriend(const char *name)
{
	SocialPlayer player;
	player.name = name;
	_session->_friends.push_back(player);
}

void GooglePlaySocialSession::finishedAddingFriends()
{
	_session->getListener()->loadFriendsEvent(SocialSessionListener::SUCCESS, _session->_friends);
}

void GooglePlaySocialSession::handleFriendsError(int errorCode)
{
	// these error codes match to GooglePlay status in GamesClient
	switch (errorCode)
	{
		case 2:  // STATUS_CLIENT_RECONNECT_REQUIRED
		case 3:  // STATUS_NETWORK_ERROR_STALE_DATA
		case 4:  // STATUS_NETWORK_ERROR_NO_DATA
		case 5:  // STATUS_NETWORK_ERROR_OPERATION_DEFERRED
		case 6:  // STATUS_NETWORK_ERROR_OPERATION_FAILED
			_session->getListener()->loadFriendsEvent(SocialSessionListener::ERROR_SERVER, _session->_friends);
			break;
		default:
			_session->getListener()->loadFriendsEvent(SocialSessionListener::ERROR_UNKNOWN, _session->_friends);
			break;

	}
}

void GooglePlaySocialSession::loadFriends()
{
	JNIEnv *env;
	jclass clazz;

	getJNI(env, clazz);

	if (env)
	{
		jmethodID method = env->GetMethodID(clazz, "loadFriends", "()V");
		if (method)
		{
			env->CallVoidMethod(__state->activity->clazz, method);
		}
		else
		{
			GP_ERROR("Couldn't find loadFriends");
		}
	}

	detachJNI();
}

void GooglePlaySocialSession::addAchievement(const char *name, const char *title, int value, int total, const char *date)
{
	SocialAchievement achievement;

	achievement.name = name;
	achievement.title = title;
	achievement.value = value;
	achievement.dateTimeAchieved = date;
	achievement.total = total;
	achievement.percentCompleted = achievement.value == 0 ? 0.0f : ((float)achievement.value / (float)achievement.total) * 100.f;

	_session->_achievements.push_back(achievement);
}

void GooglePlaySocialSession::finishedAddingAchievements()
{
	_session->getListener()->loadAchievementsEvent(SocialSessionListener::SUCCESS, _session->_achievements);
}

void GooglePlaySocialSession::handleAchievementError(int errorCode)
{
	// these error codes match to GooglePlay status in GamesClient
	switch (errorCode)
	{
		case 2:  // STATUS_CLIENT_RECONNECT_REQUIRED
		case 3:  // STATUS_NETWORK_ERROR_STALE_DATA
		case 4:  // STATUS_NETWORK_ERROR_NO_DATA
		case 5:  // STATUS_NETWORK_ERROR_OPERATION_DEFERRED
		case 6:  // STATUS_NETWORK_ERROR_OPERATION_FAILED
			_session->getListener()->loadAchievementsEvent(SocialSessionListener::ERROR_SERVER, _session->_achievements);
			break;
		default:
			_session->getListener()->loadAchievementsEvent(SocialSessionListener::ERROR_UNKNOWN, _session->_achievements);
			break;

	}
}

void GooglePlaySocialSession::loadAchievements()
{
	JNIEnv *env;
	jclass clazz;

	getJNI(env, clazz);

	if (env)
	{
		jmethodID method = env->GetMethodID(clazz, "loadAchievements", "()V");
		if (method)
		{
			env->CallVoidMethod(__state->activity->clazz, method);
		}
		else
		{
			GP_ERROR("Couldn't find loadAchievements");
		}
	}

	detachJNI();
}

const SocialAchievement* GooglePlaySocialSession::getAchievement(const char* achievementId) const
{
    uint size = _achievements.size();

    for (uint i = 0; i < size; i++)
    {
        if (strcmp(_achievements[i].name.data(), achievementId) == 0)
            return &_achievements[i];
    }
    return 0;
}

const char* GooglePlaySocialSession::getMapping(const char* identifier, const char *mapping) const
{
    const char *mappedId = identifier;

    // Lookup the mapped achievementID if there is one
    if (identifier != NULL && mapping != NULL)
    {
        Properties* mappings = _properties->getNamespace(mapping, true);
        if (mappings)
        {
            mappedId = mappings->getString(identifier);

            // in case we couldn't find the id for some reason
            if (mappedId == NULL)
                mappedId = identifier;
        }
    }

    return mappedId;
}

void GooglePlaySocialSession::submitAchievement(const char* achievementId, unsigned int value, bool achieved)
{
	JNIEnv *env;
	jclass clazz;

	getJNI(env, clazz);

	if (env)
	{
		jmethodID method = env->GetMethodID(clazz, "submitAchievement", "(Ljava/lang/String;)V");
		if (method)
		{
			const char *realAchievementId = getMapping(achievementId, "achievement_mappings");
			jstring achievementStr = env->NewStringUTF(realAchievementId);
			env->CallVoidMethod(__state->activity->clazz, method, achievementStr);
			env->DeleteLocalRef(achievementStr);
		}
		else
		{
			GP_ERROR("Couldn't find submitAchievement");
		}
	}

	detachJNI();
}

void GooglePlaySocialSession::incrementAchievement(const char* achievementId, unsigned int totalSteps)
{
	JNIEnv *env;
	jclass clazz;

	getJNI(env, clazz);

	if (env)
	{
		jmethodID method = env->GetMethodID(clazz, "incrementAchievement", "(Ljava/lang/String;I)V");
		if (method)
		{
			const char *realAchievementId = getMapping(achievementId, "achievement_mappings");
			jstring achievementStr = env->NewStringUTF(realAchievementId);
			env->CallVoidMethod(__state->activity->clazz, method, achievementStr, (jint)1);
			env->DeleteLocalRef(achievementStr);
		}
		else
		{
			GP_ERROR("Couldn't find incrementAchievement");
		}
	}

	detachJNI();
}

void GooglePlaySocialSession::synchronizeAchievements()
{
	// not supported on android
}

void GooglePlaySocialSession::addScore(int rank, const char *name, int value)
{
    SocialScore score;

    score.rank = rank;
    score.value = value;
    score.playerName = name;

    _session->_scores.push_back(score);
}

void GooglePlaySocialSession::finishedAddingScores()
{
	_session->getListener()->loadScoresEvent(SocialSessionListener::SUCCESS, _session->_scores);
}

void GooglePlaySocialSession::handleScoresError(int errorCode)
{
	// these error codes match to GooglePlay status in GamesClient
	switch (errorCode)
	{
		case 2:  // STATUS_CLIENT_RECONNECT_REQUIRED
		case 3:  // STATUS_NETWORK_ERROR_STALE_DATA
		case 4:  // STATUS_NETWORK_ERROR_NO_DATA
		case 5:  // STATUS_NETWORK_ERROR_OPERATION_DEFERRED
		case 6:  // STATUS_NETWORK_ERROR_OPERATION_FAILED
			_session->getListener()->loadScoresEvent(SocialSessionListener::ERROR_SERVER, _session->_scores);
			break;
		default:
			_session->getListener()->loadScoresEvent(SocialSessionListener::ERROR_UNKNOWN, _session->_scores);
			break;

	}
}

void GooglePlaySocialSession::loadScores(const char* leaderboardId, SocialSession::CommunityScope community, SocialSession::TimeScope time, unsigned int start, unsigned int count)
{
	JNIEnv *env;
	jclass clazz;

	getJNI(env, clazz);

	if (env)
	{
		jmethodID method = env->GetMethodID(clazz, "loadScores", "(Ljava/lang/String;IIIZ)V");

		if (method)
		{
		    const char *realLeaderboardId = getMapping(leaderboardId, "leaderboard_mappings");

			jstring leaderboardStr = env->NewStringUTF(realLeaderboardId);
			env->CallVoidMethod(__state->activity->clazz, method, leaderboardStr, (jint)time, (jint)community, (jint)count, (jboolean)false);
			env->DeleteLocalRef(leaderboardStr);
		}
		else
		{
			GP_ERROR("Couldn't find loadScores");
		}
	}

	detachJNI();
}

void GooglePlaySocialSession::loadScores(const char* leaderboardId, SocialSession::CommunityScope community, SocialSession::TimeScope time, const SocialPlayer& player, unsigned int count)
{
	JNIEnv *env;
	jclass clazz;

	getJNI(env, clazz);

	if (env)
	{
		jmethodID method = env->GetMethodID(clazz, "loadScores", "(Ljava/lang/String;IIIZ)V");
		if (method)
		{
		    const char *realLeaderboardId = getMapping(leaderboardId, "leaderboard_mappings");

			jstring leaderboardStr = env->NewStringUTF(realLeaderboardId);
			env->CallVoidMethod(__state->activity->clazz, method, leaderboardStr, (jint)time, (jint)community, (jint)count, (jboolean)true);
			env->DeleteLocalRef(leaderboardStr);
		}
		else
		{
			GP_ERROR("Couldn't find loadScores");
		}
	}

	detachJNI();
}

void GooglePlaySocialSession::submitScore(const char* leaderboardId, float score)
{
	JNIEnv *env;
	jclass clazz;

	getJNI(env, clazz);

	if (env)
	{
		jmethodID method = env->GetMethodID(clazz, "submitScore", "(Ljava/lang/String;I)V");
		if (method)
		{
		    const char *realLeaderboardId = getMapping(leaderboardId, "leaderboard_mappings");

			jstring leaderboardStr = env->NewStringUTF(realLeaderboardId);
			env->CallVoidMethod(__state->activity->clazz, method, leaderboardStr, (jint)score);
			env->DeleteLocalRef(leaderboardStr);
		}
		else
		{
			GP_ERROR("Couldn't find displayLeaderboards");
		}
	}

	_session->getListener()->submitScoreEvent(SocialSessionListener::SUCCESS);

	detachJNI();
}

void GooglePlaySocialSession::submitChallenge(const SocialPlayer *player, float score, const char* leaderboardId, unsigned int wager)
{
    SocialChallenge challenge;
    _listener->submitChallengeEvent(SocialSessionListener::ERROR_UNSUPPORTED, challenge);
}

void GooglePlaySocialSession::submitAchievementChallenge(const SocialPlayer *player, const char* achievementId, unsigned int wager)
{
    SocialChallenge challenge;
    _listener->submitChallengeEvent(SocialSessionListener::ERROR_UNSUPPORTED, challenge);
}

void GooglePlaySocialSession::loadChallenges(bool showOpenChallengesOnly)
{
	_listener->loadChallengesEvent(SocialSessionListener::ERROR_UNSUPPORTED, _challenges);
}


void GooglePlaySocialSession::replyToChallenge(const SocialChallenge *challenge, bool accept)
{
	// not supported
}

void GooglePlaySocialSession::handleLoadDataError(int errorStatus)
{
	_session->getListener()->loadSavedDataEvent(SocialSessionListener::ERROR_SERVER, "");
}

void GooglePlaySocialSession::finishedLoadingData(const char *data)
{
	_session->getListener()->loadSavedDataEvent(SocialSessionListener::SUCCESS, std::string(data));
}

void GooglePlaySocialSession::loadSavedData(const char* key)
{
	JNIEnv *env;
	jclass clazz;

	getJNI(env, clazz);

	if (env)
	{
		jmethodID method = env->GetMethodID(clazz, "cloudLoad", "(I)V");
		if (method)
		{
			int keyValue = atoi(key);
			env->CallVoidMethod(__state->activity->clazz, method, keyValue);
		}
		else
		{
			GP_ERROR("Couldn't find cloudLoad");
		}
	}

	detachJNI();
}

void GooglePlaySocialSession::submitSavedData(const char* key, std::string data)
{
	JNIEnv *env;
	jclass clazz;

	getJNI(env, clazz);

	if (env)
	{
		jmethodID method = env->GetMethodID(clazz, "cloudSave", "(ILjava/lang/String;)V");
		if (method)
		{
			int keyValue = atoi(key);
			jstring dataStr = env->NewStringUTF(data.data());
			env->CallVoidMethod(__state->activity->clazz, method, keyValue, dataStr);
			env->DeleteLocalRef(dataStr);
		}
		else
		{
			GP_ERROR("Couldn't find cloudSave");
		}
	}

	detachJNI();
}

void GooglePlaySocialSession::displayLeaderboard(const char* leaderboardId) const
{
	JNIEnv *env;
	jclass clazz;

	getJNI(env, clazz);

	if (env)
	{
		jmethodID method = env->GetMethodID(clazz, "displayLeaderboards", "(Ljava/lang/String;)V");

		if (method)
		{
		    const char *realLeaderboardId = getMapping(leaderboardId, "leaderboard_mappings");
			jstring leaderboardStr = env->NewStringUTF(realLeaderboardId);
			env->CallVoidMethod(__state->activity->clazz, method, leaderboardStr);
			env->DeleteLocalRef(leaderboardStr);
		}
		else
		{
			GP_ERROR("Couldn't find displayLeaderboards");
		}
	}

	detachJNI();
}

void GooglePlaySocialSession::displayAchievements() const
{
	JNIEnv *env;
	jclass clazz;

	getJNI(env, clazz);

	if (env)
	{
		jmethodID method = env->GetMethodID(clazz, "displayAchievements", "()V");
		if (method)
		{
			env->CallVoidMethod(__state->activity->clazz, method);
		}
		else
		{
			GP_ERROR("Couldn't find displayAchievements");
		}
	}

	detachJNI();
}

void GooglePlaySocialSession::displayChallenges() const
{
	// not supported
}

void GooglePlaySocialSession::displayChallengeSubmit(const SocialChallenge *challenge, float score) const
{
	// not supported
}

void GooglePlaySocialSession::displayPopup(const char *popupMessage, const char *title) const
{
	JNIEnv *env;
	jclass clazz;

	getJNI(env, clazz);

	if (env)
	{
		jmethodID method = env->GetMethodID(clazz, "showToast", "(Ljava/lang/String)V");
		if (method)
		{
			jstring popupStr = env->NewStringUTF(popupMessage);
			env->CallVoidMethod(__state->activity->clazz, method, popupStr);
			env->DeleteLocalRef(popupStr);
		}
		else
		{
			GP_ERROR("Couldn't find showToast");
		}
	}

	detachJNI();
}

}

#endif
