import bindings from "bindings";
const native = bindings("../build/Release/cpp-read-file-experiment-native");

export class FileReader {
  _nativeFileReader;

  constructor(_path) {
    this._nativeFileReader = new native.FileReader(_path);
  }

  read(cb) {
    this._nativeFileReader.read(cb);
  }
}
