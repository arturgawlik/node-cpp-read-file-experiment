import bindings from "bindings";
const native = bindings("./build/Release/cpp-read-file-experiment-native");

export class FileReader {
  private _nativeFileReader;

  constructor(private _path: string) {
    this._nativeFileReader = new native.FileReader();
  }

  read(cb: (content: string) => void) {
    this._nativeFileReader.read(cb);
  }
}
