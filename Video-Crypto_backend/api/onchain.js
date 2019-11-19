const router = require('express').Router();
const request = require('request');
const Config = require('../config/basic');
const executor = require('child_process').execSync;

const { Api, JsonRpc, RpcError, Numeric } = require('eosjs');
const { JsSignatureProvider } = require('eosjs/dist/eosjs-jssig');      // development only
const fetch = require('node-fetch');                                    // node only; not needed in browsers
const { TextEncoder, TextDecoder } = require('util');                   // node only; native TextEncoder/Decoder
//const { TextEncoder, TextDecoder } = require('text-encoding');          // React Native, IE11, and Edge Browsers only

const privateKeys = Config.privateKeyList;

const signatureProvider = new JsSignatureProvider(privateKeys);
const rpc = new JsonRpc('http://127.0.0.1:8000', { fetch });
const api = new Api({ rpc, signatureProvider, textDecoder: new TextDecoder(), textEncoder: new TextEncoder() });


router.post((req, res) => {
    const msg = {
        siganture: req.body.siganture
    }
    const options = {
        method: 'POST',
        url: 'http://127.0.0.1:6666/v1/wallet/list_keys',
        header: {'content-type': 'application/json'},
        body: JSON.stringify([Config.userName, Config.walletKey])
    };
    let accountInfo = {};
    request(options, (error, response, body) => {
        if (error) {
            console.log(error);
        }
        console.log('wallet response :');
        console.log(body);
        if (typeof(body) === 'string' && body[0] === '<') {
            console.log('Error: input not correct.');
            res.send({
                status: '404',
                msg: 'Blockchain cannot understand.'
            });
            return;
        }

        const pk = JSON.parse(body)[0][1];
        const sk = JSON.parse(body)[0][1];
        accountInfo.publickey = pk;
        accountInfo.privateKey = sk;

        console.log('got keys. Marking...');

        const act = {
            account: 'admin',
            name: 'sign',
            authorization: [{
                actor: Config.userName,
                permission: 'active'
            }],
            data: {
                owner: Config.userName,
                sign: msg.signature
            }
        }
        console.log(JSON.stringify(act, null ,2));
        return new Api ({
            rpc,
            signatureProvider: new JsSignatureProvider([accountInfo.privateKey]),
            textDecoder: new TextDecoder(),
            textEncoder: new TextEncoder()
        }).transact({
            actions: [act]
        }, {
            blocksBehind: 3,
            expireSeconds: 30
        }).then(value => {
            console.log(value);
            console.log('finish');
            res.send({
                status: '200',
                msg: 'OK successfully encrypted',
                id: value.body.id
            })
        })

    })
})

module.exports = router;