function Component() {} // default constructor

Component.prototype.createOperations = function()
{
    component.createOperations();
    if (systemInfo.productType === "windows") {
        // startup menu
        component.addOperation("CreateShortcut", "@TargetDir@/bin/evoplex.exe", "@StartMenuDir@/Evoplex.lnk");

        // desktop shortcut
        component.addOperation("CreateShortcut", "@TargetDir@/bin/evoplex.exe", "@DesktopDir@/Evoplex.lnk");
    }
}
