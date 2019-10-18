const router = require('express').Router()
const { spawn } = require('child_process')

router.post('/pipe', (req, res) => {
    const exec = require('child_process').execSync;

    var result = '';

    var child = exec('D:\\Working\\Reseaching\\exec.exe 114 514 1919810').toString();

    //var child = exec('ping bilibili.com').toString();
    let ret = child.split('\r\n');
    console.log(ret);
    console.log("something");
    res.send({
        status: '200',
        msg: 'OK'
    })
})


module.exports = router;