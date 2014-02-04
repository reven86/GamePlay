
Gameplay Social
===============

Summary
-------

I've added a bunch of new classes to the gameplay src directory to enable the different social platforms.  Scoreloop 
for BlackBerry, Game Center for IOS and Google Play for Android have all been implemented. Scoreloop has been set up publicly and everyone should be able to use that one.  As of right
now you will have to create the leaderboards and achievements on Game Center and Google Play yourself.  

Please refer to the spaceship sample for a working demo.

Setup
-----

1) Whichever social provider you are using go to their system and set up the leaderboards and achievements for 
your particular game.  Please refer to the documentation for Scoreloop, Game Center and Google Play for
specific information.  Check out the IOS Specific and Android Specific section below for more details.
 
2) Setup the social.config file to reference the IDs you created on the social systems giving them a unique name.
See the sample-spaceship social.config file for an example.  It is by default setup for Scoreloop integration.

i.e. here is an achievement mapping for each of the different systems:

     achievement_mappings
     {
         // Format: internalId = externalId <string>
     
         # Scoreloop 
         gameplay.spaceship.firsttime = rim.spaceship.firsttime

		  # IOS Game Center
#         gameplay.spaceship.firsttime = grp.gameplay.spaceship.firsttime
		
		# Google Play
#         gameplay.spaceship.firsttime = CgkI_P3uh-0cEAIQAQ
     }
	 
3) Define GP_USE_SOCIAL in your preprocessor definitions for your compiler.

4) Subclass your game off of SocialSessionListener and define those virtual methods for the functionality you 
wish to use.  See SpaceshipGame.h and SpaceshipGame.cpp for examples of these.

5) In your initialization code, add the following two lines to initialize the SocialController:

    if (getSocialController())
    	getSocialController()->authenticate(this);
		
6) Ensure that you have enabled the stores to enable debugging for yourself so that it will recognize you.

7) Have fun!


Android Specific
----------------

1) Go to https://play.google.com and set up your game to test.
	a) Provide your game details and link your app. (for spaceship the name is org.gameplay3d.sample_spaceship)
	b) Make note of your application ID that it provides as you will need to enter this in ids.xml (i.e. 991348637436)
	c) Enter your achievements and also make note of the achievement ID's (needed for social.config file)
	d) Enter one or more leaderboards and copy the unique google ID's (needed to enter in social.config)
	e) Add yourself as a tester.
2) Install the Google Play Services through the Android SDK Manager.
3) Make a copy the Google Play Services library to your own location.
4) In the copy, add build properties in order to compile it yourself.
5) Run ant debug on it.
6) Set up a reference to your copy of the Google Play Services library by following the instructions below:

	http://developer.android.com/tools/projects/projects-cmdline.html#ReferencingLibraryProject
	
My project.properties file ended up looking something like:

# Project target.
target=android-17
android.library.reference.1=../../../../google-play-services/
android.library.reference.2=../../../../google-play-services/
android.library.reference.3=../../../../google-play-services/
	
Note - If you wish to modify the headers that are used for JNI calls simply run the following command:

javah -v -classpath <gameplay-path>\samples\spaceship\android\bin\classes;c:\<android-sdk-path>\sdk\platfo
rms\android-17\android.jar;<gameplay-path>\samples\spaceship\android\libs\google-play-services.jar -o GooglePlaySocialJNI.h 
org.gameplay3d.sample_spaceship.GooglePlaySocial


IOS Specific
------------

1) Go to https://developer.apple.com/ and click on the member center and then click on iTunes connect.
2) Go to manage your apps and click Add New App.  You will have to do this twice, once for IOS and once for MacOSX if
you have two versions.  (If you are creating two versions and they share the same achievements and leaderboards then you 
choose to group them together.  This is what the spaceship sample is doing).
3) You will create a bundle ID here which needs to match what's in the .plist file 
(i.e. org.gameplay3d.sample-spaceship-ios for the ios spaceship sample)
4) Click on Manage Game Center button and proceed to add your leaderboards and achievements.
5) Note the IDs that you create as you will have to use them in the social.config file.
6) The bundle ID you created earlier, make sure you edit the .plist file and make sure it's the same.

i.e. for spaceship the sample-spaceship-ios.plist CFBundleIdentifier entry looks like:

<key>CFBundleIdentifier</key>
	<string>org.gameplay3d.${PRODUCT_NAME:rfc1034identifier}</string>

Note - We are using some depracated methods as the latest ios version now requires you to write your own interfaces
for a lot of things.  The code should be fairly straight forward to change.	




