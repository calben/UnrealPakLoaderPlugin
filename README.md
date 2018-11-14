# Unreal Runtime Package Loader Plugin

PakLoader is a library that simplifies runtime package loading for Unreal by providing a handful of static functions that abstract Unreal Engine's pak file system.
The simplest function can open a pak file and load its asset registry to your project's given the path to a `.pak` file and its associated `.bin` file.

Unreal Engine has a reasonably good package and modding system.
Unfortunately, it is largely undocumented, so individuals attempting to use it to load pak files at runtime are left to their own devices (trial and error, mostly) to discover how to get a pak file to load properly.
Below is a project checklist for ensuring your project is ready to go for pak loading at runtime, but a more thorough explanation is available in the [documentation](https://calben.github.io/unrealpakloaderplugin/).

While some of the content in the documentation may be very obvious to you, I suggest going ahead and giving it all a quick skim as you might find some of the notes will make your life easier.

## Project Checklist for Pak Loading

+ Your pak file is prepared with the same version of Unreal as your project
+ Your project is packaging with "compress contents to pak file" or the `-pak` flag

## Loading Pak Files

After you're sure your project is correctly set up, 

## Debugging When Using Pak File Loading at Runtime

Unfortunately, loading pak files in this way adds greater difficulty to debugging, as you cannot runtime load pak files while in the editor.
You must debug by packaging your project and then running `DebugGame` builds.
However, if you try to run DebugGame and load a package, you're going to get the following error:

```
Error: Assertion failed: PakPrecacherSingleton [File:D:\Build\++UE4+Release-4.19+Compile\Sync\Engine\Source\Runtime\PakFile\Private\IPlatformFilePak.cpp] [Line: 1017]
```

Thanks to Epic's Gil Gribb and Ben Marsh, we now know this is because your debugging instance's base directory needs to be set to be your packaged project's directory.
Add the following to the build command.

```
-BaseDir={{path_to_packaged_project}}/WindowsNoEditor/{{your_project_name}}/Binaries/Win64/
```

## Credits

<div>Icons made by <a href="https://www.freepik.com" title="Freepik">Freepik</a> from <a href="https://www.flaticon.com/" title="Flaticon">www.flaticon.com</a> is licensed by <a href="http://creativecommons.org/licenses/by/3.0/" title="Creative Commons BY 3.0" target="_blank">CC 3.0 BY</a></div>
