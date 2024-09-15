import { FileReader } from "../lib/file-reader.js";
try {
  const fileReader = new FileReader(
    "/home/parallels/programming/node-cpp-read-file-experiment/package.json"
  );
  fileReader.read((res) => {
    console.log("js callback called with: ", res);
  });
} catch (err) {
  console.error(err);
}
