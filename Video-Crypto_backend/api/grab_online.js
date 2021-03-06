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

/*
* take a signature out from blockchain.
* input: id, output: signature.
* required to login.
* */

router.post('/', (req, res) => {
    const msg = {
        id: req.body.id
    }
    try {
        if (Config.userName === null || Config.walletKey === null) {
            console.log('User has not login. Refused to serve.');
            res.send({
                status: 403,
                message: 'Please sign in before any other operations.'
            });
            return;
        }
        console.log('Got requests. Now Grabbing online...');
        const row = {
            json: true,
            code: 'admin',
            table: 'video',
            lower_bound: msg.id,
            scope: 'admin',
            limit: 1,
            // reverse: true,
            // show_payer: true
        }
        request({
            method: 'POST',
            headers: {'content-type': 'application/json'},
            url: 'http://127.0.0.1:8000/v1/chain/get_table_rows',
            body: JSON.stringify(row)
        }, (error, response, body) => {
            if (error) {
                console.log(error);
                res.send({
                    status: '403',
                    msg: 'request error'
                })
                //return;
            }
            console.log(JSON.stringify(JSON.parse(body), null, 2));
            const signature = JSON.parse(body).rows[0].sign;
            res.send({
                status: '200',
                msg: 'OK successfully get the signature',
                signature: signature
            })
        });
        /*
        return rpc.get_table_rows(rows).then((value) => {
            console.log(JSON.stringify(value, null, 2));
            const signature = value.rows[0].data.sign;
            res.send({
                status: '200',
                msg: 'OK successfully get the signature',
                signature: signature
            })
        }).catch(error => {
            console.log(error);
            res.send({
                status: '500',
                msg: 'unexpected error'
            })
        })
        */
    } catch(e) {
        res.send({
            status: '500',
            msg: 'Something went wrong.'
        })
    }
});


module.exports = router;