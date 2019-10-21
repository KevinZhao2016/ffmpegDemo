const router = require('express').Router();
const request = require('request');

router.get('/', (req, res) => {
    const options = {
        method: 'POST',
        url: 'http://127.0.0.1:3000/api/blank/giver',
        headers: {'content-type': 'application/json'},
        body: JSON.stringify({
            input: 'giver',
            output: 'jace, the mind sculpter',
        })
    };
    console.log('get piped');
    let ret = { first_value: 'foo' };
    request(options, (error, response, body) => {
        if (error) console.log(error);
        console.log(body);
        ret = JSON.parse(body).ret;
        res.send({
            status: '200',
            msg: ret
        });
    })

});

module.exports = router;