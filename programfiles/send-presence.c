/*
This is a simple example in C of using the rich presence API asyncronously.
*/

#define _CRT_SECURE_NO_WARNINGS /* thanks Microsoft */
#define DELIM "="

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "discord-rpc.h"

int fixConfigInput(char * input, char * output);

char APPLICATION_ID[26];
static int FrustrationLevel = 0;
static int64_t StartTime;
char discordConserveStartTime[64];
char discordCustomStartTime[64];
char discordState[256];
char discordDetails[256];
char discordLargeImageKey[64];
char discordSmallImageKey[64];
char discordSmallImageText[512];
char discordLargeImageText[256];
int64_t customStartTime = 0;
char discordConserveStartTimeRaw[256];
char discordCustomStartTimeRaw[265];
char APPLICATION_IDRAW[70];
char discordStateRaw[256];
char discordDetailsRaw[256];
char discordLargeImageKeyRaw[64];
char discordSmallImageKeyRaw[64];
char discordSmallImageTextRaw[512];
char discordLargeImageTextRaw[256];
//int timer;
FILE *config;

static int prompt(char* line, size_t size)
{
	int res;
	char* nl;
	printf("\n> ");
	fflush(stdout);
	res = fgets(line, (int)size, stdin) ? 1 : 0;
	line[size - 1] = 0;
	nl = strchr(line, '\n');
	if (nl) {
		*nl = 0;
	}
	return res;
}
static void updateDiscordPresence()
{
	//some of the old code
	//char buffer[256];
	//sprintf(buffer, "Frustration level: %d", FrustrationLevel);    
	//discordPresence.endTimestamp = time(0) + 5 * 60;
	//discordPresence.smallImageKey = "ptb-small";
	//discordPresence.partyId = "party1234";
	//discordPresence.partySize = 1;
	//discordPresence.partyMax = 6;
	//discordPresence.joinSecret = "join";
	//discordPresence.spectateSecret = "look";


	DiscordRichPresence discordPresence;
	memset(&discordPresence, 0, sizeof(discordPresence));
	discordPresence.state = discordState;
	discordPresence.details = discordDetails;
	discordPresence.startTimestamp = StartTime;
	//discordPresence.largeImageText = discordLargeImageText;
	discordPresence.largeImageKey = discordLargeImageKey;
	if (discordLargeImageText != NULL)
	{
		discordPresence.largeImageText = discordLargeImageText;
	}
	if (discordSmallImageText != NULL)
	{
		discordPresence.smallImageText = discordSmallImageText;
	}
	if (discordSmallImageKey != NULL)
	{
		discordPresence.smallImageKey = discordSmallImageKey;
	}
	if (discordDetails != NULL)
	{
		discordPresence.details = discordDetails;
	}
	discordPresence.matchSecret = "xyzzy";
	discordPresence.instance = 0;
	Discord_UpdatePresence(&discordPresence);
}
static void handleDiscordReady()
{
	printf("\nDiscord: ready\n");
}
static void handleDiscordDisconnected(int errcode, const char* message)
{
	printf("\nDiscord: disconnected (%d: %s)\n", errcode, message);
}
static void handleDiscordError(int errcode, const char* message)
{
	printf("\nDiscord: error (%d: %s)\n", errcode, message);
}
static void handleDiscordJoin(const char* secret)
{
	printf("\nDiscord: join (%s)\n", secret);
}
static void handleDiscordSpectate(const char* secret)
{
	printf("\nDiscord: spectate (%s)\n", secret);
}
static void handleDiscordJoinRequest(const DiscordJoinRequest* request)
{
	int response = -1;
	char yn[4];
	printf("\nDiscord: join request from %s - %s - %s\n",
		request->username,
		request->avatar,
		request->userId);
	do {
		printf("Accept? (y/n)");
		if (!prompt(yn, sizeof(yn))) {
			break;
		}

		if (!yn[0]) {
			continue;
		}

		if (yn[0] == 'y') {
			response = DISCORD_REPLY_YES;
			break;
		}

		if (yn[0] == 'n') {
			response = DISCORD_REPLY_NO;
			break;
		}
	} while (1);
	if (response != -1) {
		Discord_Respond(request->userId, response);
	}
}
static void discordInit()
{
	DiscordEventHandlers handlers;
	memset(&handlers, 0, sizeof(handlers));
	handlers.ready = handleDiscordReady;
	handlers.disconnected = handleDiscordDisconnected;
	handlers.errored = handleDiscordError;
	handlers.joinGame = handleDiscordJoin;
	handlers.spectateGame = handleDiscordSpectate;
	handlers.joinRequest = handleDiscordJoinRequest;
	Discord_Initialize(APPLICATION_ID, &handlers, 1, NULL);
}
static void gameLoop()
{
	char line[512];
	char* space;

	//sets it to like 17 hours
	StartTime = time(0) - customStartTime;

	printf("Welcome to the Discord Rich Presence-o-bot!\n");
	printf("q = quit, y = reinit\n");
	printf("please press 'a' for the program to hook to discord!\n");
	while (prompt(line, sizeof(line))) {
		if (line[0]) {
			if (line[0] == 'q') {
				break;
			}

			if (line[0] == 't') {
				printf("Shutting off Discord.\n");
				Discord_Shutdown();
				continue;
			}

			if (line[0] == 'y') {
				printf("Reinit Discord.\n");
				discordInit();
				continue;
			}
			if (line[0] == 'g')
			{
				printf("RUNNING CONFIG GENERATOR");
				configGen(1);
				continue;
			}
			if (line[0] == 'a')
			{
				printf("Attempting to bind to Discord!");
			}
			if (line[0] == 'r')
			{
				reboot();
			}
			if (line[0] == 'p')
			{
				int lengthtest = strlen(discordLargeImageKey);
				printf("%s%s%s%s%s%s", discordLargeImageKey, discordSmallImageKey, discordLargeImageText, discordSmallImageText, discordDetails, discordState);
			}
			if (time(NULL) & 1) 
			{
				if (line[0] != 'a')
				{
					printf("I don't understand that.\n");
				}
			}
			else 
			{
				space = strchr(line, ' ');
				if (space) 
				{
					*space = 0;
				}
				printf("I don't know the word \"%s\".\n", line);
			}

			++FrustrationLevel;

			updateDiscordPresence();
		}

#ifdef DISCORD_DISABLE_IO_THREAD
		Discord_UpdateConnection();
#endif
		Discord_RunCallbacks();
	}
}
int countlines(char *filename)
{
	FILE *fp = fopen(filename, "r");
	int ch = 0;
	int lines = 0;

	while (!feof(fp))
	{
		ch = fgetc(fp);
		if (ch == '\n')
		{
			lines++;
		}
	}

	return lines;
}
int reboot()
{
	system("send-presence.exe");
	abort;
	return 0;
}
int readConfig()
{
	config = fopen("config.txt", "r");
	char buf[1000];
	int i = 0;
	if (!config) { return 1; }
	while (fgets(buf, 1000, config) != NULL)
	{
		if (i == 0)
		{
			strcpy(APPLICATION_IDRAW, buf);
		}
		if (i == 1)
		{
			strcpy(discordLargeImageKeyRaw, buf);
		}
		if (i == 2)
		{
			strcpy(discordSmallImageKeyRaw, buf);
		}
		if (i == 3)
		{
			strcpy(discordLargeImageTextRaw, buf);
		}
		if (i == 4)
		{
			strcpy(discordSmallImageTextRaw, buf);
		}
		if (i == 5)
		{
			strcpy(discordDetailsRaw, buf);
		}
		if (i == 6)
		{
			strcpy(discordStateRaw, buf);
		}
		if (i == 7)
		{
			strcpy(discordConserveStartTimeRaw, buf);
		}
		if (i == 8)
		{
			strcpy(discordCustomStartTimeRaw, buf);
		}
		i++;
	}
	fclose(config);

	//deletes the \n after each line of input
	if (APPLICATION_ID[strlen(APPLICATION_ID) - 1] == '\n')
	{
		APPLICATION_ID[strlen(APPLICATION_ID) - 1] = '\0';
	}
	if (discordLargeImageKey[strlen(discordLargeImageKey) - 1] == '\n')
	{
		discordLargeImageKey[strlen(discordLargeImageKey) - 1] = '\0';
	}
	if (discordSmallImageKey[strlen(discordSmallImageKey) - 1] == '\n')
	{
		discordSmallImageKey[strlen(discordSmallImageKey) - 1] = '\0';
	}
	if (discordCustomStartTime[strlen(discordCustomStartTime) - 1] == '\n')
	{
		discordCustomStartTime[strlen(discordCustomStartTime) - 1] = '\0';
	}

	fixConfigInput(APPLICATION_IDRAW, APPLICATION_ID);
	fixConfigInput(discordLargeImageKeyRaw, discordLargeImageKey);
	fixConfigInput(discordSmallImageKeyRaw, discordSmallImageKey);
	fixConfigInput(discordLargeImageTextRaw, discordLargeImageText);
	fixConfigInput(discordSmallImageTextRaw, discordSmallImageText);
	fixConfigInput(discordDetailsRaw, discordDetails);
	fixConfigInput(discordStateRaw, discordState);
	fixConfigInput(discordCustomStartTimeRaw, discordCustomStartTime);
	fixConfigInput(discordConserveStartTimeRaw, discordConserveStartTime);
	fixConfigInput(discordCustomStartTimeRaw, discordCustomStartTime);

	return 0;
}
int countTime(int result)
{
	result = time(0) - StartTime;
}
int fixConfigInput(char * input, char * output)
{
	int start;
	int end;
	int count = 0;
	for (int i = 0; i < strlen(input); i++)
	{
		if (input[i] == '\"')
		{
			if (count == 0)
			{
				start = i;
			}
			if (count == 1)
			{
				end = i;
			}
			count++;
		}//end if
	}//end for
	if (count < 1)
	{
		strcpy(output, input);
		return 0;
	}
	//now I have the start and end points of the string
	int j = 0;
	for (int n = start + 1; n <= end - 1; n++)
	{
		output[j] = input[n];
		j++;
	}
	int last = end - start;
	output[last - 1] = '\0';
	return 1;
}
int writeConfig()
{
	clearConfig();
	config = fopen("config.txt", "a");
	fprintf(config, "%s\n", discordLargeImageKey);
	fprintf(config, "%s\n", discordSmallImageKey);
	fprintf(config, "%s\n", discordLargeImageText);
	fprintf(config, "%s\n", discordSmallImageText);
	fprintf(config, "%s\n", discordDetails);
	fprintf(config, "%s\n", discordState);
	fprintf(config, "%s\n", discordConserveStartTime);
	if (strstr(discordConserveStartTime, "y"))
	{
		int tmp = 0;
		countTime(tmp);
		fprintf(config, "%s\n", tmp);
	}
	else
	{
		fprintf(config, "%s\n", "0");
	}
	fclose(config);
	return 0;
}
int clearConfig()
{
	config = fopen("config.tex", "w");
	fclose(config);
	return 0;
}
int checkConfig()
{
	FILE * config;
	//config = fopen("config.txt", "w");
	//fclose(config);
	config = fopen("config.txt", "r");
	char buf[1000];
	int i = 0;
	if (!config) { return 1; }
	while (fgets(buf, 1000, config) != NULL)
	{
		if (strstr(buf, "DISCORD APP CLIENT ID"));
		{
			fclose(config);
			return 1;
		}
	}
	return 0;
}
int configGen(int bypasss)
{
	if (bypasss == 1 || (checkConfig() == 0 && bypasss == 0))
	{
		FILE * config;
		//add generation of config file
		//fclose(config);
		config = fopen("config.txt", "w");
		fprintf(config, "DISCORD APP CLIENT ID: \"\"\n");
		fprintf(config, "LARGE IMAGE KEY: \"\"\n");
		fprintf(config, "SMALL IMAGE KEY: \"\"\n");
		fprintf(config, "LARGE IMAGE HOVER TEXT: \"\"\n");
		fprintf(config, "SMALL IMAGE HOVER TEXT: \"\"\n");
		fprintf(config, "DETAILS: \"\"\n");
		fprintf(config, "STATUS: \"\"\n");
		fprintf(config, "CONSERVE START TIME? (Y/N): \"\"\n");
		fprintf(config, "IF YOU DON'T WANT A CUSTOM START TIME, ENTER 0 OR LEAVE BLANK: \"\"\n");	
	}
	if (checkConfig() == 0 && bypasss == 0)
	{
		FILE * config;
		//add generation of config file
		//fclose(config);
		config = fopen("config.txt", "w");
		fprintf(config, "DISCORD APP CLIENT ID: \"\"\n");
		fprintf(config, "LARGE IMAGE KEY: \"\"\n");
		fprintf(config, "SMALL IMAGE KEY: \"\"\n");
		fprintf(config, "LARGE IMAGE HOVER TEXT: \"\"\n");
		fprintf(config, "SMALL IMAGE HOVER TEXT: \"\"\n");
		fprintf(config, "DETAILS: \"\"\n");
		fprintf(config, "STATUS: \"\"\n");
	}
}
int main(int argc, char* argv[])
{
	configGen(0);

	readConfig();

	//parseConfigInput();

	gameLoop();

	discordInit();

	Discord_Shutdown();
	return 0;
}
