function Component() {} // default constructor

Component.prototype.createOperations = function()
{
    component.createOperations();
    if (systemInfo.productType === "windows") {
        // startup menu
        component.addOperation("CreateShortcut", "@TargetDir@/evoplex.exe", "@StartMenuDir@/Evoplex.lnk");

        // desktop shortcut
        component.addOperation("CreateShortcut", "@TargetDir@/evoplex.exe", "@DesktopDir@/Evoplex.lnk");
    }
}
