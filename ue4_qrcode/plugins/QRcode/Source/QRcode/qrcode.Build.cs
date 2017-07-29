// Some copyright should be here...

using UnrealBuildTool;

public class QRcode : ModuleRules
{
	public QRcode(TargetInfo Target)
	{
		
		PublicIncludePaths.AddRange(
			new string[] {
				"QRcode/Public"
				
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				"QRcode/Private",
                "QRcode/Private/LibQREncode"
				
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",

                "ImageWrapper",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",

				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
