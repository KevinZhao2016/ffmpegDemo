const router = require('express').Router()
const { spawn } = require('child_process')
const Config = require('../config/basic')

router.post('/pipe', (req, res) => {
    const exec = require('child_process').execSync;

    var result = '';

    var child = exec('D:\\Working\\Reseaching\\exec.exe 114 514 1919810').toString();
    console.log("${Config.relative_path} + 'and Shameimaru saying hello to you.'")
    //var child = exec('ping bilibili.com').toString();
    let ret = child.split('\r\n');
    console.log(ret);
    console.log("something");
    res.send({
        status: '200',
        msg: 'OK'
    })
})

router.post('/giver', (req, res) => {
    const ret = {
        name: req.body.input + req.body.output,
        value: "lilliana, the last hope"
    }
    console.log('giver got it.')

    res.send({
        status: '200',
        ret: ret
    })
})


module.exports = router;