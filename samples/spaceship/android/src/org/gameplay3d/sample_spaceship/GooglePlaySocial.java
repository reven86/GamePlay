    
package org.gameplay3d.sample_spaceship;

import java.util.List;
import java.util.ArrayList;
import java.sql.Date;
import android.app.NativeActivity;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.widget.Toast;
import com.google.android.gms.games.GamesClient;
import com.google.android.gms.games.Player;
import com.google.android.gms.games.PlayerBuffer;
import com.google.android.gms.games.achievement.Achievement;
import com.google.android.gms.games.achievement.AchievementBuffer;
import com.google.android.gms.games.achievement.OnAchievementsLoadedListener;
import com.google.android.gms.games.OnPlayersLoadedListener;
import com.google.android.gms.games.leaderboard.LeaderboardBuffer;
import com.google.android.gms.games.leaderboard.LeaderboardScore;
import com.google.android.gms.games.leaderboard.LeaderboardScoreBuffer;
import com.google.android.gms.games.leaderboard.LeaderboardVariant;
import com.google.android.gms.games.leaderboard.OnLeaderboardScoresLoadedListener;
import com.google.android.gms.appstate.AppStateClient;
import com.google.android.gms.appstate.OnStateLoadedListener;
import com.google.android.gms.games.GamesClient;

public class GooglePlaySocial extends NativeActivity 
	implements GameHelper.GameHelperListener, OnLeaderboardScoresLoadedListener, OnAchievementsLoadedListener, OnStateLoadedListener
{    private static final int RC_UNUSED = 11999;
	private static List<Player> mFriends = new ArrayList<Player>();
	private static final String TAG = "gameplay.GooglePlaySocial";
    private static final int FRIENDS_PER_PAGE = 10;
    private static final int MAX_SCORES_TO_LOAD = 25;
	private static boolean mCalledAuthenticate = false;
	public static GameHelper mHelper = null;
	
	// Google+ ID of the currently signed-in user
    private Player mPlayer = null;
	   
    // cloud save data we loaded
    private String mCloudData;
	
	// Load the game's native code:
	static 
	{
		System.loadLibrary("sample-spaceship");
	}
	
	// We call this method to report the sign in state to the native code
    private native void native_ReportSignInState(boolean isSignedIn, boolean isInProgress);
	
	// We call this method to report the host user info
	private native void native_ReportUserEntry(String playerName, String playerId);
	
	// We call this method to report an achievement to the native code
    private native void native_ReportAchievementEntry(int errorStatus, String name, String title, String dateAchieved, int value, int total);
	
	// We call this method to report a score to the native code
    private native void native_ReportLeaderboardEntry(int errorStatus, int index, String playerName, String playerId, int score);
	
	// We call this method to report a friend to the native code
    private native void native_ReportFriendEntry(int errorStatus, String playerName, String playerId);
		
	// we call this method to inform native code of the result of the cloud load operation
    private native void native_ReportCloudLoadResult(int errorStatus, String savedData);

	
	/**
     * Called when Activity is created. Here, we create and set up the GameHelper
     * object.
     */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        // create the GameHelper
        mHelper = new GameHelper(this);

        // always enable debug logs, since this is a sample. For a production application, this
        // might not be a good approach.
        mHelper.enableDebugLog(true, TAG);

        // setup the helper. We request the GamesClient and the AppStateClient since
        // we're using the games API and also the Cloud Save API. If we also needed
        // Google+, we could add GameHelper.CLIENT_PLUS.
        mHelper.setup(this, GameHelper.CLIENT_GAMES | GameHelper.CLIENT_APPSTATE);
    }

    /**
     * Called when Activity is started. Every time the Activity is started, we need
     * to restart the sign-in process. So we report this fact to native code and
     * begin the sign-in process via GameHelper.
     */
    @Override
    public void onStart() {
        super.onStart();
        // report that sign in is in progress (remember that every time the Activity gets
        // onStart(), the sign in process has to start again).
        native_ReportSignInState(false, true);

        mHelper.onStart(this);
    }


    /**
     * Called when Activity is stopped. Here, we inform the GameHelper that the Activity
     * has stopped, and report this fact to the native code as well.
     */
    @Override
    public void onStop() {
        super.onStop();

        // no longer signed in, but we're in process of recovering it (and will be until
        // the app returns to the foreground)
        native_ReportSignInState(false, true);

        mHelper.onStop();
    }


    /**
     * Called when Activity gets a result. We simply pass it along to GameHelper.
     * This is necessary because GameHelper uses secondary activities during the
     * sign-in flow, and needs to get the results for those.
     */
    @Override
    public void onActivityResult(int request, int response, Intent data) {
        super.onActivityResult(request, response, data);
        mHelper.onActivityResult(request, response, data);
    }


    /**
     * Called when sign in succeeds. From this point on, we can make calls to the
     * Play Games API. We report the success to native code.
     */
    @Override
    public void onSignInSucceeded() {
		GamesClient gamesClient = mHelper.getGamesClient();
        mPlayer = gamesClient.getCurrentPlayer();
		
        // report that sign-in has succeeded
		if (mCalledAuthenticate)
		{
			native_ReportSignInState(true, false);
			native_ReportUserEntry(mPlayer.getDisplayName(), mPlayer.getPlayerId());
		}
    }

    /**
     * Called when sign in fails. From this point on, we cannot make calls to the
     * Play Games API. We report the failure to native code.
     */
    @Override
    public void onSignInFailed() {
		mPlayer = null;
        // report that sign-in has failed
        native_ReportSignInState(false, false);
    }
	
	public void gameServicesSignIn() 
	{
		mCalledAuthenticate = true;
		
        runOnUiThread(new Runnable() 
		{
            public void run() {
                mHelper.beginUserInitiatedSignIn();
            }
        });
    }
	
	public boolean isSignedIn()
    {
        return (mHelper != null && mHelper.isSignedIn());
    }

    public void submitScore(String leaderboardId, int score) 
	{
		if (isSignedIn())
			mHelper.getGamesClient().submitScore(leaderboardId, score);
    }

	public void submitAchievement(String achievementId)
	{
        if (isSignedIn())
		{
            mHelper.getGamesClient().unlockAchievement(achievementId);
        }
    }
	
	public void incrementAchievement(String achievementId, int percentage)
	{
        if (isSignedIn())
		{
            mHelper.getGamesClient().incrementAchievement(achievementId, percentage);
        }
    }

    public void displayLeaderboards(final String leaderboardId) 
	{
		if (isSignedIn())
		{
			runOnUiThread(new Runnable() 
			{
				public void run() {
					if (leaderboardId != null)
						startActivityForResult(mHelper.getGamesClient().getLeaderboardIntent(leaderboardId), RC_UNUSED);
					else
						startActivityForResult(mHelper.getGamesClient().getAllLeaderboardsIntent(), RC_UNUSED);
				}
			});
		}
		else
		{
			gameServicesSignIn();
		}
    }

    public void displayAchievements() 
	{
		if (isSignedIn())
		{
			runOnUiThread(new Runnable() 
			{
				public void run() {
					startActivityForResult(mHelper.getGamesClient().getAchievementsIntent(), RC_UNUSED);
				}
			});
		}
		else
		{
			gameServicesSignIn();
		}
    }

    public void loadFriends() 
	{
        if (mFriends.size() > 0) 
		{
            mFriends.clear();
        }

        runOnUiThread(new Runnable() 
		{
            public void run() 
			{        
				mHelper.getGamesClient().loadInvitablePlayers(new OnPlayersLoadedListener() 
				{
					@Override
					public void onPlayersLoaded(int statusCode, PlayerBuffer playerBuffer) 
					{
						if (statusCode == GamesClient.STATUS_OK) 
						{
							for (Player player : playerBuffer) 
							{
								mFriends.add(player);
							}

							if (playerBuffer.getCount() == FRIENDS_PER_PAGE) 
							{
								mHelper.getGamesClient().loadMoreInvitablePlayers(this, FRIENDS_PER_PAGE);
							} 
							else 
							{
								// call out and return all the friends <more code to call into C++>
								for (Player friend : mFriends) 
								{
									native_ReportFriendEntry(GamesClient.STATUS_OK, friend.getDisplayName(), friend.getPlayerId());
								}
								
								// this will signal the end of the friends
								native_ReportFriendEntry(GamesClient.STATUS_OK, null, null);
							}
						}
						else
						{
							native_ReportFriendEntry(statusCode, null, null);
						}
						
						playerBuffer.close();
					}
				}, FRIENDS_PER_PAGE, false);
            }
        });
    }
	
	/**
     * Called when leaderboard scores are loaded. 
     */
    @Override
    public void onLeaderboardScoresLoaded(int statusCode, LeaderboardBuffer leaderboard,
            LeaderboardScoreBuffer scores) {

        // STATUS_OK means we got fresh data from the server;
        // NETWORK_ERROR_STALE_DATA means we got locally cached data. Both are
        // good enough for our purposes.
        boolean success = (statusCode == GamesClient.STATUS_OK || 
						   statusCode == GamesClient.STATUS_NETWORK_ERROR_STALE_DATA);

        if (!success) 
		{
            // something went wrong...
			native_ReportLeaderboardEntry(statusCode, -1, null, null, 0);
            return;
        }

        int i;
        for (i = 0; i < scores.getCount(); i++) 
		{
            LeaderboardScore thisScore = scores.get(i);
            String playerId = thisScore.getScoreHolder().getPlayerId();
            int score = (int) thisScore.getRawScore();
            String name = thisScore.getScoreHolderDisplayName();	
			native_ReportLeaderboardEntry(GamesClient.STATUS_OK, i, playerId, name, score);
        }
		// signal the end of the list
		native_ReportLeaderboardEntry(GamesClient.STATUS_OK, i, null, null, -1);
		
		leaderboard.close();
		scores.close();
    }
	
	public void loadScores(String leaderboardId, int timeType, int communityType, int count, boolean aroundPlayer) 
	{
		GamesClient gamesClient = mHelper.getGamesClient();
		
		int community = (communityType == 0) ? LeaderboardVariant.COLLECTION_SOCIAL : LeaderboardVariant.COLLECTION_PUBLIC;
		
		if (aroundPlayer)
		{
			gamesClient.loadPlayerCenteredScores(this, leaderboardId, timeType, community, count);
		}
		else
		{
			gamesClient.loadTopScores(this, leaderboardId, timeType, community, count);
		}
	}
	
	/**
     * Called when achievements are loaded. 
     */
    @Override
    public void onAchievementsLoaded(int statusCode, AchievementBuffer achievementBuffer) 
	{
        // STATUS_OK means we got fresh data from the server;
        // NETWORK_ERROR_STALE_DATA means we got locally cached data. Both are
        // good enough for our purposes.
        boolean success = (statusCode == GamesClient.STATUS_OK || 
						   statusCode == GamesClient.STATUS_NETWORK_ERROR_STALE_DATA);

        if (!success) 
		{
            // something went wrong...
			native_ReportAchievementEntry(statusCode, null, null, null, 0, 0);
            return;
        }

        int i;
		int count = achievementBuffer.getCount();
        for (i = 0; i < count; i++) 
		{
		    int value = 0;
			int total = 0;
            Achievement achievement = achievementBuffer.get(i);
			
			String id = achievement.getAchievementId();
            String title = achievement.getName();
			if (achievement.getType() == Achievement.TYPE_INCREMENTAL)
			{
				value = achievement.getCurrentSteps();
				total = achievement.getTotalSteps();
			}

			long timeStamp = achievement.getLastUpdatedTimestamp();
			Date dateAchieved = new Date(timeStamp);
			native_ReportAchievementEntry(GamesClient.STATUS_OK, id, title, dateAchieved.toString(), value, total);
        }
		
		native_ReportAchievementEntry(GamesClient.STATUS_OK, null, null, null, -1, -1);
		achievementBuffer.close();
    }
	
	public void loadAchievements() 
	{
		GamesClient gamesClient = mHelper.getGamesClient();
		gamesClient.loadAchievements(this, true);	
	}
	
		
	public void cloudLoad(int stateKey)
	{
	Log.d(TAG, "cloudLoad " + stateKey);
		mHelper.getAppStateClient().loadState(this, stateKey);
	}
	
	/**
     * Requests that data be saved to the cloud. Can be called from any thread. This method
     * is called from native code to save the level to the cloud.
     *
     * @param level The level to save to the cloud.
     */
	public void cloudSave(final int stateKey, final String data)
	{
	Log.d(TAG, "cloudSave " + stateKey + " data " + data);
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                AppStateClient client = mHelper.getAppStateClient();
                if (client.isConnected()) 
				{
                    Log.d(TAG, "Saving level to cloud: " + data);
                    client.updateState(stateKey, data.getBytes());
                }
            }
        });
    }
	
    /**
     * Called when state is loaded from the cloud. We analyze the response code and
     * report appropriately to native code.
     */
    @Override
    public void onStateLoaded(int statusCode, int stateKey, byte[] data) 
	{
        Log.d(TAG, "State loaded, status = " + statusCode);
        switch (statusCode) 
		{
            case AppStateClient.STATUS_OK:
            case AppStateClient.STATUS_NETWORK_ERROR_STALE_DATA:
                // we have good data -- which is either it's fresh (STATUS_OK) or
                // from the local cache (STALE_DATA), but either way it's good enough for us:
				String loadData = new String(data);
				mCloudData = loadData;
				native_ReportCloudLoadResult(AppStateClient.STATUS_OK, mCloudData);
				break;
				
            case AppStateClient.STATUS_STATE_KEY_NOT_FOUND:
                // KEY_NOT_FOUND means we never saved anything, so empty the string and return it
				mCloudData = "";
				native_ReportCloudLoadResult(AppStateClient.STATUS_OK, mCloudData);
                break;
				
            case AppStateClient.STATUS_NETWORK_ERROR_OPERATION_DEFERRED:
                // This is returned after saving state to the cloud if the state could not
                // be pushed to the server immediately. 
            case AppStateClient.STATUS_NETWORK_ERROR_NO_DATA:
                // There was a network error and no local data was available.
				native_ReportCloudLoadResult(statusCode, null);
                break;
            default:
                // There was an error loading data. Warn the user.
				native_ReportCloudLoadResult(statusCode, null);
                break;
        }
    }

    /**
     * Called when there's a cloud conflict. Our conflict resolution logic is pretty
     * straightforward: just take the local data
     */
    @Override
    public void onStateConflict(int stateKey, String resolvedVersion, byte[] localData, byte[] serverData) 
	{
        mHelper.getAppStateClient().resolveState(this, stateKey, resolvedVersion, localData);
    }
}