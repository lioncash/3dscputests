#include <3ds.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/iosupport.h>

static FILE* f = NULL;

static ssize_t file_write(struct _reent* r, int fd, const char* ptr, size_t len)
{
	if (len > 1)
	{
		fprintf(f, "%.*s", len, ptr);
	}
	return len;
}

static const devoptab_t dotab_file = {
	"file",
	0,
	NULL,
	NULL,
	file_write,
	NULL,
	NULL,
	NULL
};

static bool TryOpeningNewFile(const char* path)
{
	f = fopen(path, "w");
	return (f != NULL);
}

// Prototypes
void DumpCP15Registers(bool);
void IntegerMain(void);
void ThumbMain(void);
void V6MediaMain(void);
void VFPMain(void);

// Used to store the CPSR processor mode string.
// Visually displaying what mode the processor is in can be helpful.
static const char* GetCPSRMode()
{
	unsigned int cpsr = 0;
	asm volatile ("MRS %[out], CPSR" : [out]"=&r"(cpsr));

	// The mode bits in the CPSR are the bottom 5 bits.
	unsigned int mode_bits = (cpsr & 0x1F);

	// These are the only relevant modes to check for.
	if (mode_bits & 16)
		return "User";
	if (mode_bits & 31)
		return "System";

	// If we hit here, something is very off.
	return "3DS is drunk";
}

int main(int argc, char **argv)
{
	// Initialize services
	srvInit();
	aptInit();
	hidInit(NULL);
	gfxInitDefault();
	fsInit(); //needed for filesystem stuff
	sdmcInit();

	PrintConsole topScreen;
	PrintConsole bottomScreen;

	consoleInit(GFX_TOP, &topScreen);
	consoleInit(GFX_BOTTOM, &bottomScreen);
	consoleSelect(&topScreen);

	// ctrulib doesn't actually expose their devoptab for console output so we have to store that
	// ourselves, as we flip between writing to the SD card and printing text to the console.
	// Kinda crappy, but meh.
	const devoptab_t console_tab = *devoptab_list[STD_OUT];

	printf("ARM Instruction Tests based off of Valgrind tests.\n\n\n");
	printf("Current Processor Mode: %s\n\n\n", GetCPSRMode());
	printf("Press A to run ARMv6 media tests.\n");
	printf("Press B to run ARM integer tests.\n");
	printf("Press X to run ARM VFP tests.\n");
	printf("Press Y to run ARM Thumb tests.\n");
	printf("Press Select to run all above tests.\n\n\n");
	printf("            -- Miscellaneous --\n");
	printf("Press R to dump user accessible CP15 registers.\n");
	printf("Press L to dump all CP15 registers (need Sys mode)\n");
	printf("Press Start to exit.\n");

	// We display status text on the bottom display.
	consoleSelect(&bottomScreen);

	// TODO: Below could probably be cleaned up a little.
	while (aptMainLoop())
	{
		hidScanInput();
		const u32 kDown = hidKeysDown();

		// break in order to return to hbmenu
		if (kDown & KEY_START)
			break;

		if (kDown & KEY_A)
		{
			if (TryOpeningNewFile("sdmc:/media_tests.txt"))
			{
				printf("Running media tests...\n");
				devoptab_list[STD_OUT] = &dotab_file;
				V6MediaMain();
				devoptab_list[STD_OUT] = &console_tab;
				printf("Done!\n");
				fclose(f);
			}
			else
			{
				printf("Unable to create media tests output file.\n");
			}
		}

		if (kDown & KEY_B)
		{
			if (TryOpeningNewFile("sdmc:/integer_tests.txt"))
			{
				printf("Running integer tests...\n");
				devoptab_list[STD_OUT] = &dotab_file;
				IntegerMain();
				devoptab_list[STD_OUT] = &console_tab;
				printf("Done!\n");
				fclose(f);
			}
			else
			{
				printf("Unable to create integer tests output file.\n");
			}
		}

		if (kDown & KEY_X)
		{
			if (TryOpeningNewFile("sdmc:/vfp_tests.txt"))
			{
				printf("Running VFP tests...\n");
				devoptab_list[STD_OUT] = &dotab_file;
				VFPMain();
				devoptab_list[STD_OUT] = &console_tab;
				printf("Done!\n");
				fclose(f);
			}
			else
			{
				printf("Unable to create VFP tests output file.\n");
			}
		}

		if (kDown & KEY_Y)
		{
			if (TryOpeningNewFile("sdmc:/thumb_tests.txt"))
			{
				printf("Running Thumb tests...\n");
				devoptab_list[STD_OUT] = &dotab_file;
				ThumbMain();
				devoptab_list[STD_OUT] = &console_tab;
				printf("Done!");
				fclose(f);
			}
			else
			{
				printf("Unable to create output file.\n");
			}
		}

		if (kDown & KEY_SELECT)
		{
			if (TryOpeningNewFile("sdmc:/all_tests.txt"))
			{
				printf("Running all tests...\n");
				devoptab_list[STD_OUT] = &dotab_file;
				V6MediaMain();
				IntegerMain();
				ThumbMain();
				VFPMain();
				devoptab_list[STD_OUT] = &console_tab;
				printf("Done!\n");
			}
			else
			{
				printf("Unable to create output file for all tests.\n");
			}
		}

		if (kDown & KEY_R)
		{
			if (TryOpeningNewFile("sdmc:/user_cp15_regs.txt"))
			{
				printf("Dumping user-accessible CP15 registers...\n");
				devoptab_list[STD_OUT] = &dotab_file;
				DumpCP15Registers(false);
				devoptab_list[STD_OUT] = &console_tab;
				printf("Done!\n");
			}
			else
			{
				printf("Unable to create CP15 regs output file.\n");
			}
		}

		if (kDown & KEY_L)
		{
			if (TryOpeningNewFile("sdmc:/all_cp15_regs.txt"))
			{
				printf("Dumping all CP15 registers...\n");
				devoptab_list[STD_OUT] = &dotab_file;
				DumpCP15Registers(true);
				devoptab_list[STD_OUT] = &console_tab;
				printf("Done!\n");
			}
			else
			{
				printf("Unable to create CP15 regs output file.\n");
			}
		}

		gfxSwapBuffers();
		gfxFlushBuffers();
		gspWaitForVBlank();
	}

	// Exit services
	sdmcExit();
	fsExit();
	gfxExit();
	hidExit();
	aptExit();
	srvExit();
	return 0;
}

