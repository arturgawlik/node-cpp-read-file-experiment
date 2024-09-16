import { FileReader } from "../lib/file-reader.js";

try {
  const fileReader = new FileReader(
    "/home/parallels/programming/node-cpp-read-file-experiment/package.json"
  );
  fileReader.read((res) => {
    console.log(res);
  });
} catch (err) {
  console.error(err);
}
