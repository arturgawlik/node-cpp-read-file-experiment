import { FileReader } from "../lib/file-reader";

const fileReader = new FileReader(
  "/home/parallels/programming/node-cpp-read-file-experiment/tsconfig.json"
);
fileReader.read((res) => {
  console.log(res);
});
