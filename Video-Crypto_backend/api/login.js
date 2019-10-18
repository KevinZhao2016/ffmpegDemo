const router = require('express').Router();
const request = require('request');
const Config = require('../config/basic');

const { Api, JsonRpc, RpcError, Numeric } = require('eosjs');
const { JsSignatureProvider } = require('eosjs/dist/eosjs-jssig');      // development only
const fetch = require('node-fetch');                                    // node only; not needed in browsers
const { TextEncoder, TextDecoder } = require('util');                   // node only; native TextEncoder/Decoder
//const { TextEncoder, TextDecoder } = require('text-encoding');          // React Native, IE11, and Edge Browsers only

const privateKeys = Config.privateKeyList;

const signatureProvider = new JsSignatureProvider(privateKeys);
const rpc = new JsonRpc('http://127.0.0.1:8000', { fetch });
const api = new Api({ rpc, signatureProvider, textDecoder: new TextDecoder(), textEncoder: new TextEncoder() });

router.post('/', (req, res) => {
    const msg = {
        walletName : req.body.walletName,
        walletKey : req.body.walletKey
    }
    try {
        const options = {
            method : 'POST',
            url : 'http://127.0.0.1:6666/v1/wallet/unlock',
            header : {'content-type': 'application/x-www-form-urlencoded; charset=UTF-8'},
            body : JSON.stringify([msg.walletName, msg.walletKey])
        }
        console.log('Fetching the wallet. Params');
        console.log(options.body);
        request(options, (error, response, body) => {
            try {
                if (error) throw new Error(error);
                console.log(body);
                if (typeof (body) === 'string' && body[0] === '<') {
                    throw new Error('smart server error!');
                }
                const ret = JSON.parse(body)
                if (ret === {} || ret.error.what === 'Already unlocked') {
                    Config.userName = req.body.walletName;
                    Config.walletKey = req.body.walletKey;
                    /*
                    res.cookie('walletKey', req.body.walletKey);
                    res.cookie('userName', req.body.name);
                    */
                    console.log('wallet response:');
                    console.log(body);
                    console.log('ok unlocked the wallet.');
                    res.send({
                        status: 200,
                        msg: 'ok unlocked the wallet.',
                    });
                } else {
                    console.log('Failed to unlock the wallet: The walletKey is not correct');
                    res.send({
                        status: 500,
                        msg: 'The key of wallet is wrong. Please check your password.'
                    });
                }
            } catch(e) {
                console.log(e);
                res.send({
                    status: 500,
                    message: e.message
                });
                return;
            }
        })
    } catch (e) {
        console.log(e);
        res.send({
            'status' : 500,
            'message' : 'Cannot login and unlock wallet. Some Problems happened.'
        });
    }
})

module.exports = router