const router = require('express').Router()
const { spawn } = require('child_process')

router.post('/pipe', (req, res) => {
    const exec = require('child_process').execSync;

    var result = '';

    var child = exec('ping bilibili.com').toString();
    console.log(child);
    console.log("something");
    res.send({
        status: '200',
        msg: 'OK'
    })
})


module.exports = router;