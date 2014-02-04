#include "Social.h"
#include "Base.h"
#include "SocialChallenge.h"

namespace gameplay
{

SocialChallenge::SocialChallenge()
    : state(INVALID), score(0.0f), opponentScore(0.0f)
{
}

SocialChallenge::~SocialChallenge()
{
}

}
