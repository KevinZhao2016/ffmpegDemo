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
* display all the videos' messages from the blockchain.
* */

router.post('/', (req, res) => {
    try {
        const rows = {
            json: true,
            code: 'admin',
            table: 'video',
            scope: 'admin',
            limit: 1,
            reverse: true,
            show_payer: true
        }
        let ret = [];
        request({
            method: 'POST',
            headers: {'content-type': 'application/json'},
            url: 'http://127.0.0.1:8000/v1/chain/get_table_rows',
            body: JSON.stringify(rows)
        }, (error, response, body) => {
            if (error) {
                console.log(error);
                res.send({
                    status: '403',
                    msg: 'request error'
                })
            }
            console.log(JSON.stringify(JSON.parse(body), null, 2));
            const list = JSON.parse(body).rows;
            const len = list.length;
            for( i = 0; i < len; i++) {
                ret.push({
                    ID: list[i].ID,
                    owner: list[i].owner,
                    signature: list[i].sign,
                    time: list[i].time
                })
            }
            res.send({
                status: '200',
                list: ret
            })

        })

    } catch (e) {
        console.log(e);
        res.send({
            status: '500',
            msg: 'something went wrong.'
        })
    }
});


module.exports = router;