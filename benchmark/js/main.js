import express from "express"

const app = express();

app.get("/", (req, res) => {
  res.send("This is the data to be sent back to the client!!!!")
});

app.listen(4000);
