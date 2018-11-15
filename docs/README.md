# Unreal Runtime Package Loader Plugin

PakLoader is a library that simplifies runtime package loading for Unreal by providing a handful of static functions that abstract Unreal Engine's pak file system.
The simplest function can open a pak file and load its asset registry to your project's given the path to a `.pak` file and its associated `.bin` file.

Unreal Engine has a reasonably good package and modding system.
Unfortunately, it is largely undocumented, so individuals attempting to use it to load pak files at runtime are left to their own devices (trial and error, mostly) to discover how to get a pak file to load properly.
Below is a project checklist for ensuring your project is ready to go for pak loading at runtime, but a more thorough explanation is available in the [documentation](https://calben.github.io/unrealpakloaderplugin/).

While some of the content in the documentation may be very obvious to you, I suggest going ahead and giving it all a quick skim as you might find some of the notes will make your life easier.

## Adding the Plugin to Your Project

This repository contains only the plugin with its contents at the root directory.
You may either:

1. Copy paste the plugin into your Plugins directory under "PakLoader/".
1. Clone this plugin directly into your Plugins directory.
1. Use `git submodule add https://github.com/calben/UnrealPakLoaderPlugin Plugins/PakLoader` to maintain this plugin in your project using Git.

## Project Checklist for Pak Loading

+ Your pak file is prepared with the same version of Unreal as your project
+ Your main project is packaging with "compress contents to pak file" or the `-pak` flag
+ Your pak file has been built either for your project or for "any project"
+ Your pak file has its "AssetRegistry.bin" next to it, renamed to "{{ pak_file_name }}.bin".

## Preparing Your Pak Files

You may either create pak files using the Project Launcher inside the editor or by running the automation tool yourself.
I recommend creating a batch file or a utility script that will call on the automation tool, as then your team will be reliably using the same build settings.

### Preparing with the Project Launcher

1. Go to the project launcher and create a new custom launch profile
1. Give it a name
1. Under "Build," ensure the build option is checked with the appropriate configuration
1. Under "Cook," check by the book for the appropriate platform.  For desktop editions, this would be the "{{ platform }}NoEditor" edition.
1. Under "Release / DLC / Patching Settings" uncheck "Create a release..." and instead enter the release number of your main project under Release version this is based on.  Just underneath this, check "Build DLC."  And under "Name of the DLC to build" type the name of your plugin.
1. Under "Advanced Settings," check "Store all content in a single file (UnrealPak).
1. Under "Package," select an option for packaging.  If you're not sure what to put here, then you probably want to package and store locally and leave the packaging location as the default one.
1. Under "Deploy," select "Do not deploy."

After these steps have been completed, you may run the launch profile to generate the pak file for your pak file.

### Preparing with a Script

These are recommended parameters when building your pak files.
Depending on where you are in your project, you may want to switch serverconfig to Shipping.
You may also need to remove the no compile flag, depending on your needs.

```
UE_{{ version }}\\Engine\\Binaries\\DotNET\\AutomationTool.exe

-ScriptsForProject=\"{{ uproject_file_path }}\"
BuildCookRun
-project=\"{{ uproject_file_path }}\"
-dlcname=\"{{ plugin_name }}\"
-noP4 -clientconfig=Development -serverconfig=Development -nocompile -nocompileeditor -installed -ue4exe=UE4Editor-Cmd.exe -utf8output
-platform=Win64 -targetplatform=Win64 -build -cook -unversionedcookedcontent -pak -DLCIncludeEngineContent -basedonreleaseversion=1.0 -compressed -stage -package
```

An important item to note is the `-DLCIncludeEngineContent`.
In theory, this shouldn't be necessary, but I have been unable to correctly package plugins without this flag for Unreal Engine up to and including 4.19.

## Loading Pak Files

### Notes On The `FPakPlatformFile*`

Most of the functions in this library either take in an `FPakPlatformFile` pointer or return one.
You need to keep the  pointer alive until you are done with your pak file.
If you allow your pointer to fall out of scope and later try to access the contents of the pak file, you're going to run into undefined behaviour.
When Unreal Engine attempts to load the contents of the pak file, it will attempt to do so through this pointer.
Thus, if it has fallen out of scope, one of the below is likely to happen:

1. Everything works fine.  The value in the address hasn't been changed yet, so consider yourself lucky.
1. `TryLoad` fails without any specific errors.  If you're not checking the value of the `UObject` pointer that was meant to be returned, you're in for a hard crash.
1. If the object is a texture file, it's possible that your texture will be filled with garbage data.  I'm still not entirely sure what's going on in this case except that your pointer must have been changed to an address determined to be a valid location containing data that is then read as a raw texture.  I'm surprised Unreal doesn't make more checks to guard against this, but it's a mystery to solve another day!

### Temporary Loading of Single Pak File 

After you're sure your project is correctly set up, you can load a pak using `LoadPakFileAndAddToRegistry(FString PakFilePath, FString PakMountDirectory)`.
By default, you probably want the mount directory and pak file path to be equivalent, for example `auto PakPlatformFile = LoadPakFileAndAddToRegistry("./Test.pak", "Test")`.
If you're only loading a pak file for a short period, then this method will do the trick.
After the routine completes, you will be able to access the contents of the pak file in your asset registry under `/Test/*`.

### Loading Multiple Paks or Keeping Paks Loaded




## Debugging When Using Pak File Loading at Runtime

Unfortunately, loading pak files in this way adds difficulty when debugging, as you cannot runtime load pak files while in the editor.
You must debug by packaging your project and then running `DebugGame` builds.
However, if you try to run DebugGame and load a package, you're going to get the following error:

```
Error: Assertion failed: PakPrecacherSingleton [File:D:\Build\++UE4+Release-4.19+Compile\Sync\Engine\Source\Runtime\PakFile\Private\IPlatformFilePak.cpp] [Line: 1017]
```

Thanks to Epic's Gil Gribb and Ben Marsh, we now know this may be fixed by setting your debugging instance's base directory to your packaged project's directory.
You may do this by adding the following to the build command.

```
-BaseDir={{path_to_packaged_project}}/WindowsNoEditor/{{your_project_name}}/Binaries/Win64/
```

In Visual Studio, this would be done by opening the properties for the project, either by right clicking the project root and clicking "Properties" or by selecting the desired project and using the hotkey alt + enter, navigating to the configuration properties for debugging, and adding this line, as shown in the image below.

![Project Properties Setup Image](_media/ProjectPropertiesBaseDir.png ':size=600px')

## Credits

- 
- Gil Grib and Ben Marsh at Epic Games for their help with debugging
- <div>Icons made by <a href="https://www.freepik.com" title="Freepik">Freepik</a> from <a href="https://www.flaticon.com/" title="Flaticon">www.flaticon.com</a> is licensed by <a href="http://creativecommons.org/licenses/by/3.0/" title="Creative Commons BY 3.0" target="_blank">CC 3.0 BY</a></div>
