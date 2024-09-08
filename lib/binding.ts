const addon = require('../build/Release/cpp-read-file-experiment-native');

interface ICppReadFileExperimentNative
{
    greet(strName: string): string;
};

class CppReadFileExperiment {
    constructor(name: string) {
        this._addonInstance = new addon.CppReadFileExperiment(name)
    }

    greet (strName: string) {
        return this._addonInstance.greet(strName);
    }

    // private members
    private _addonInstance: ICppReadFileExperimentNative;
}

export = CppReadFileExperiment;
