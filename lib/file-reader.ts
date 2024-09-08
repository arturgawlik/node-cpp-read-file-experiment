import bindings from "bindings";
const native = bindings();

export class FileReader {
    private _nativeFileReader;
  
    constructor(private _path: string) {
        this._nativeFileReader = new ;
    }

    read(cb: (content: string) => void) {
        this._nativeFileReader.read(cb);
    }
}
