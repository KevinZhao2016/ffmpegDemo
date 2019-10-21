const router = require('express').Router();
const register = require('./register');
const login = require('./login');
const encrypt = require('./encrypt');
const decrypt = require('./decrypt')
const generate = require('./generate');
const display = require('./display');
const grab_offline = require('./grab_offline');
const grab_online = require('./grab_online');
const certificate = require('./certificate');
const blank = require('./blank')
const test = require('./test')

router.use('/register', register);
router.use('/login', login);
router.use('/encrypt', encrypt);
router.use('/decrypt', decrypt);
router.use('/generate', generate);
router.use('/display', display);
router.use('/grab-offline', grab_offline);
router.use('/grab-online', grab_online);
router.use('/certificate', certificate);
router.use('/blank', blank);
router.use('/test', test);

module.exports = router;