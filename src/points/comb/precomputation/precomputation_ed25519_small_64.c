#if !FE3C_SMALL_PRECOMPUTATION
    #error "Build system inconsistency detected! precomputation_ed25519_small_64.c in use despite FE3C_SMALL_PRECOMPUTATION not being set"
#endif /* !FE3C_SMALL_PRECOMPUTATION */

#if !FE3C_64BIT
    #error "Build system inconsistency detected! precomputation_ed25519_64.c in use despite FE3C_64BIT not being set"
#endif /* !FE3C_64BIT */

#include <points/comb/comb_ed25519.h>

const ed25519_precomp_internal ed25519_comb_precomp[32][8] = {
    {
        {
            .X = { 0x62d608f25d51a, 0x412a4b4f6592a, 0x75b7171a4b31d, 0x1ff60527118fe, 0x216936d3cd6e5 },
            .Y = { 0x6666666666658, 0x4cccccccccccc, 0x1999999999999, 0x3333333333333, 0x6666666666666 },
        },
        {
            .X = { 0x5a14e2843ce0e, 0xa2baf48bf078, 0xcf9eb0203639, 0x2361e821dbe8c, 0x36ab384c9f5a0 },
            .Y = { 0x746ae6af8a3c9, 0x22c870a2ac1cb, 0x6887d5a5ce43d, 0x4e10ed12f7464, 0x2260cdf309232 },
        },
        {
            .X = { 0x2485fd3f8e25c, 0x3302c4910d58c, 0x36b20e98d0e60, 0x7a48ffa573a1f, 0x67ae9c4a22928 },
            .Y = { 0x3684878f5b4d4, 0x2ece480608058, 0x9a7bde7c5bb0, 0x4d5d09350c730, 0x1267b1d177ee6 },
        },
        {
            .X = { 0x2a657c4c9f870, 0x3279c2a8e927, 0xd483e469ce7b, 0xa34192ea5c3d, 0x203da8db56cff },
            .Y = { 0xab61ca32112f, 0x65d45e1fe1be7, 0x355c5b133c8a0, 0x2f0a3875c42c0, 0x47d0e827cb159 },
        },
        {
            .X = { 0x9cc0322ef233, 0x727c37c34b228, 0x4b6977970a067, 0x43dfe77be7be8, 0x49fda73eade35 },
            .Y = { 0x21f83d676c8ed, 0x15128616ba21a, 0x6491998c4a0bb, 0x737f016370a44, 0x5f4825b298fea },
        },
        {
            .X = { 0x2741a7dcbf23d, 0x4d8f6884ef07, 0x428a6fa879666, 0xe315756606e, 0x4c9797ba7a456 },
            .Y = { 0x27ad0f9497ef4, 0xd289ad6c183a, 0x53df5dfe505f0, 0x4508edb84d3fe, 0x54de3fc2886d },
        },
        {
            .X = { 0x5981af50e4107, 0x6777e39d2ab0a, 0x476041e0fa027, 0x6a774f1f70ca5, 0x14568685fcf4b },
            .Y = { 0x4c4b59f4062b8, 0xdef57e47a258, 0x4dab507c220ad, 0x297c3e732346e, 0x31c563e32b47d },
        },
        {
            .X = { 0x7fdbc08a584c8, 0x7700d31732770, 0x13b3e4faceb19, 0xdb214316ae7c, 0x6742e15f97d77 },
            .Y = { 0x75ba9fc37b9b4, 0x78c43dc9263c5, 0x22bce3e05e0f3, 0x1bcb756b784b3, 0x21d30600c9e57 },
        },
    },
    {
        {
            .X = { 0xbcdd0cc2a556, 0x776ac2aeda417, 0x241512752d5e3, 0x65ff374e5e093, 0x5e7e07ed4e1de },
            .Y = { 0x42031566cf6c7, 0x35824fb3501c2, 0x74c9839f46cf6, 0x13cb5faf5d540, 0xf55755c51f10 },
        },
        {
            .X = { 0x1333c82cd340a, 0x366072c3a4a81, 0x1c3509887bff6, 0x2399998fcf77d, 0x2eecbf81b3d08 },
            .Y = { 0x4f764008b93e8, 0x1093cdc0e9713, 0x28466d9213535, 0x13e20996cdba0, 0x54d2dc2da1378 },
        },
        {
            .X = { 0x6cef314cc93bb, 0x554a7aa3728ac, 0x73636f95fab74, 0x71452d53d8c3c, 0x70b225147463b },
            .Y = { 0x67392b18d9c02, 0x9bb22c14b73, 0x535826851e4d0, 0x200e1303393fc, 0x244980ded32ff },
        },
        {
            .X = { 0x1c7d55b4a2b68, 0x89655c23b22a, 0x32ecdeaf3c51f, 0x682c8563e5b64, 0x6b113a52f9f3f },
            .Y = { 0x54359f327c13d, 0x69eaf32d8b212, 0x5467018a59657, 0x73c975c6d0c02, 0x1aeb017159697 },
        },
        {
            .X = { 0x2fde6e9a95bd2, 0x507fcc850e338, 0x64101647815fb, 0xbcd6b508ee9c, 0x661b75757abbc },
            .Y = { 0x597e42a7b09be, 0x2fadb6255c92f, 0x601f01f443255, 0x58978670117a7, 0x5da1ae1b5d4b },
        },
        {
            .X = { 0x23c8ab6490615, 0x55862840685fd, 0x196f4f8b2bf5a, 0x7f59da5a6f110, 0x603ba4adc34a9 },
            .Y = { 0x22a95c0e658bc, 0xda5e6f535025, 0x11b1432f122f8, 0x66c87d66a3570, 0x1fdbaf2c3482c },
        },
        {
            .X = { 0x540310b391d99, 0x37a2582d266d3, 0x3e78cc3af5aa1, 0x1d22e8f0ad79d, 0x3d718e644bf2d },
            .Y = { 0x63bf5a23600f9, 0xefe3ee501511, 0x8aa2497e40ab, 0x20a179144089d, 0x320d055dbba3f },
        },
        {
            .X = { 0x2bcfbbd1317f7, 0x52622b668bd9a, 0x318a0f6120abf, 0x1c5daa0fa88fe, 0x3c1ae7527612d },
            .Y = { 0x15a20711364e, 0x16c5865588a24, 0x517e43e99fd7a, 0x2bf5bc308b642, 0x51c91c3f9d7e },
        },
    },
    {
        {
            .X = { 0x5678a8411a565, 0x68f4d089a33d3, 0x1f35528e7f84, 0x997aa0fbce5, 0x5de7faa2ee4e7 },
            .Y = { 0x5e4cef854ec36, 0x11203adfedbf0, 0x50cb64b03a2f0, 0x70555c754c935, 0x2c9f23641e1c2 },
        },
        {
            .X = { 0x411b172d04f27, 0x7d0291282a4e2, 0x474754cb61d1, 0x29b991b31b014, 0x74e337a01c13b },
            .Y = { 0x66796114e25f1, 0x35c4a7658439c, 0x3cd667ecbba17, 0x57cb767fd5edd, 0x8648f02b3a4d },
        },
        {
            .X = { 0x14e4390d7c290, 0x7c926f3dd69bb, 0x7ff2072205381, 0x17950cbd3d0b7, 0x73ce78c748a87 },
            .Y = { 0x5e13a0a0cb2ca, 0x7a7f77532928f, 0x58381c33ab994, 0x7637f1cb1e5fc, 0x33c172e606062 },
        },
        {
            .X = { 0x7475464f29434, 0x15744114800e6, 0x496e91cff5d55, 0x42af416580a11, 0x6a6fbd51efa6a },
            .Y = { 0x206b27616d671, 0x7ca58789eaf3d, 0x5e1c2b7874585, 0x615be7c088b0e, 0x16f5f8529f55f },
        },
        {
            .X = { 0x5532a8f320ca8, 0x1bf5987794688, 0x75dd2cb15d929, 0x5affba5f75d88, 0x455d905f52d04 },
            .Y = { 0x460470fe6a8b5, 0x594d8050a8cae, 0x31e44ec7bbe6e, 0x395b4de113dd8, 0x33dbf271d5fc },
        },
        {
            .X = { 0x4643dca20439a, 0x3c0923d355e4c, 0x1b6ad9d5c1952, 0xb9bfb82f8d48, 0x584e7f2a5e367 },
            .Y = { 0xfe4e547c1381, 0x1d81ecbb2c45f, 0x3a6e210cd1cc8, 0x7f4091979ede0, 0x2e5327982a30d },
        },
        {
            .X = { 0x657c725de8fce, 0x5ad57b534cccd, 0x78821744738bf, 0x5ea70641cec63, 0x7783d619f52f8 },
            .Y = { 0x5cfc4600c5c4d, 0x34201b0471600, 0x7a7aff5114eb0, 0x4ff41a1091ecc, 0x7b87c33701d60 },
        },
        {
            .X = { 0x4e0c5f69a9634, 0x3d5a981c48062, 0x3c6ca588dc26a, 0x2aa4d9961ce97, 0x243eb188de73a },
            .Y = { 0x7b25b1f4ce2f6, 0x821039f4b055, 0x2c893328b6fbc, 0x16c00f5bd05bf, 0x3779298951f45 },
        },
    },
    {
        {
            .X = { 0xcf27d8d43a77, 0x1bbb953ae543d, 0x5095bb5e76a58, 0x13358e63b00a, 0xaf367956af63 },
            .Y = { 0x1bd2f2ba408c5, 0x1c96d4c55026, 0x25ac2c8f76a44, 0x1d9b846e94567, 0x21108d900134d },
        },
        {
            .X = { 0x7074fbae70d33, 0x2d8b40fbd51bb, 0x56c6d5f96c2c7, 0x5ad320a09a2fa, 0x2b760154fc5ce },
            .Y = { 0x63696afc20c02, 0x56d440a9c495f, 0x68b3b5d8a7ecb, 0x1701b9a08901f, 0x7dc81af96fed4 },
        },
        {
            .X = { 0x23f36e811b9d8, 0x11e3a7b995828, 0x675ed453cf48f, 0x279d550726c46, 0x4535c89311d60 },
            .Y = { 0x7bf9008d63661, 0x2b081ed92ef54, 0x201ab24dcb4cf, 0x470d551051ec8, 0x69dd3661ba84b },
        },
        {
            .X = { 0x492c69c5bdb6b, 0x70eb718b5e477, 0x65212db5be90b, 0x69c3c2449cc31, 0x67dbc33418bbc },
            .Y = { 0x7b056093af396, 0x1072363ca2f8d, 0x1ea88943327a9, 0x1982cfe4973e5, 0x38436fdf8589a },
        },
        {
            .X = { 0x249b3ebdd1011, 0x12f437b17b475, 0x7173f5d884de5, 0x586375e7b19c4, 0x69f0b68125b9a },
            .Y = { 0x37a47dc3669d6, 0x2ec78aa467a8d, 0x78cb7f911386b, 0x3a3487665627d, 0xd2a89a19c281 },
        },
        {
            .X = { 0x55ffa5112f8eb, 0x2a063fd4ce313, 0x27e3d4f7f2a35, 0x2166846366960, 0x34d9729a3945d },
            .Y = { 0x3cf404c6279b1, 0x66c9cec06e1b6, 0x385a1c1f00554, 0x1275a30aa7728, 0x65efef05bd03d },
        },
        {
            .X = { 0x30403c96f4e4c, 0xf2fcc2114b1c, 0x7219fd127c222, 0x1414193b15434, 0x271b64006d3d0 },
            .Y = { 0x45df50bdd7d9, 0x7207dbd956650, 0x7600ec14fbcb9, 0xe17e3247881c, 0x6e0746310b9c0 },
        },
        {
            .X = { 0x4e9bd19448479, 0x515054dd8188a, 0x17c89c7550e6e, 0x3dc59effd00ee, 0x29ea44a50a4b4 },
            .Y = { 0x55f63af509005, 0x3f2e713bc33d3, 0x5aac918c2b1b0, 0x5465deda5e88c, 0x6c775ac8ab2ed },
        },
    },
    {
        {
            .X = { 0x797a46abc0cbb, 0x20e5bcde5b262, 0xca003cb02070, 0x462eed5ea13b4, 0x4d1e116d13615 },
            .Y = { 0x61efb10c9b91a, 0x76bd149709e89, 0x4023e310fa12d, 0x2db2e6289aa5a, 0x6d415be49d4e3 },
        },
        {
            .X = { 0x2148248127d15, 0x658297a8659a8, 0x3e38bb230a985, 0x3eedf71ca2292, 0x4b5bae5a77a86 },
            .Y = { 0x3b8de209a77cb, 0x4af7181f41b24, 0x19d0bb2dabb2e, 0x90cc3ad22c07, 0x739476e0b3847 },
        },
        {
            .X = { 0x67e5a18ecc8f9, 0xf8e7374be3e2, 0x20b53c6b986ae, 0x278281c2d3acc, 0x28e01e9eebe62 },
            .Y = { 0x682a0a58a68c1, 0x6d36c8bc54f6e, 0x5c412374e86d3, 0x14db149d3e0d5, 0x34a3ef2080894 },
        },
        {
            .X = { 0x33f232869fcdf, 0x3be66e4877f0b, 0x2a26341ac8b54, 0x789f0837fe7b6, 0x33bfa90cc1b9d },
            .Y = { 0x5824fb2da81d9, 0x3a5e21302873, 0x405f4b7d2b3bc, 0x572cfd2fff884, 0x53b120db6327c },
        },
        {
            .X = { 0x70b4a1110cc99, 0x77b444058d29e, 0x29aecabcdb38a, 0x64388b7fb4166, 0x6759a4f961d6a },
            .Y = { 0x43f61ecf8af35, 0x4c0b7c2cc6794, 0xdb52c40468, 0x263d92c8908b0, 0x7e6c7c2dca5f4 },
        },
        {
            .X = { 0x434a9ade80267, 0x1b8f55022be1a, 0x1339dd7aa521d, 0xf3c3b487024a, 0x56bec70310915 },
            .Y = { 0x66b33ec86fb32, 0x60d9f59056a25, 0x5c4301739f91d, 0x4666e532b5a79, 0x6ab2ddd075077 },
        },
        {
            .X = { 0x3d6530a7c82bc, 0x4f6bd651a0841, 0xa7158c176b82, 0x33ed6f9d40bd0, 0x4bbefbdc2c608 },
            .Y = { 0x63f6849538107, 0x420e11412a081, 0xf9d3deb0d51f, 0x6e4bce8e72ee6, 0x6eb85cc89c0c2 },
        },
        {
            .X = { 0x222bc0b9efc6, 0x386b66e0e23c1, 0x3da69124805ea, 0x6814de0caac58, 0x444929347c2de },
            .Y = { 0x42aff2e1d6245, 0x181af64b48423, 0x695f23ff0e456, 0x5eb22d1928f3e, 0x77785ec5cbbda },
        },
    },
    {
        {
            .X = { 0x490fa3b4710e5, 0x1eacdfd56a61f, 0x6cb20ee9c3bee, 0x549ca19bd017f, 0xf6c3a96e0032 },
            .Y = { 0x479be167635a, 0x4d2f58f2713ef, 0x10893b9457875, 0x33a81c525ac74, 0x3d4e97e286378 },
        },
        {
            .X = { 0x69fade6abb440, 0x55551d6694d28, 0x6317458822a73, 0x692628efb5e89, 0x3b13c71c2a90b },
            .Y = { 0x5be180d16dc54, 0x5f0004a4c2c86, 0x23f4d0a9bdc16, 0x3d95dd4133c97, 0x14890a7ab2481 },
        },
        {
            .X = { 0x320f5fd1d642b, 0x298630e9dbbe6, 0x54627423f3186, 0x6bc3fb34ab459, 0x7983fccb8fdff },
            .Y = { 0x163226a436855, 0x4aaf356b5777b, 0x2e94120f7867b, 0x6661493ab8ed7, 0x3c0fcdb8f1b0c },
        },
        {
            .X = { 0x6c091e388a820, 0x6a3044f1576d, 0x69bae0ea4a2ca, 0x2a112c8ba6d4, 0x184b48cb45500 },
            .Y = { 0x24dba26e77c33, 0x4707f5e8a7fc6, 0x603bdcd99e697, 0x1d4eeb0613411, 0x3ee0e07a03dc3 },
        },
        {
            .X = { 0x62364b859c113, 0x2a53f9591e9b0, 0x6861d4b0ffd5a, 0x7f8ec47a30758, 0x5d17bc7ae6348 },
            .Y = { 0x7f1ed5765a23d, 0x614aa29e2f0b9, 0x67865bd10140e, 0x17b0b1984d6ea, 0x53daa8c1f0668 },
        },
        {
            .X = { 0x747ad656376ce, 0x14a7699747e90, 0x1cda466130882, 0x364aa3e07070d, 0x2d86e1368ab54 },
            .Y = { 0x14429f04394f2, 0x353c3eee86778, 0x797e122085254, 0x5dcb2acc4850, 0xb2538616310b },
        },
        {
            .X = { 0x4369a69769d1, 0xd259eb635c91, 0x3966e2502aaa9, 0xaaba846341f8, 0x5260385be8429 },
            .Y = { 0x4164a69a476bd, 0x4c7f153c9ce4c, 0x440c6a3b8e442, 0x18e8239f60811, 0x3d894f1c3c8b5 },
        },
        {
            .X = { 0xe3c0fb135c34, 0x6d3644b0a856f, 0xf096724dfe05, 0x2841310cb74e9, 0x5a04ae1195edf },
            .Y = { 0x31f9712c5e8b9, 0x5ba933287dfd0, 0x29a0971654be4, 0x66676743f3af8, 0x676667092118a },
        },
    },
    {
        {
            .X = { 0x799fbc504fd52, 0x685c319f7d1c, 0x3dd367b6677bc, 0x4a2cde61767c0, 0x52a837bc7a7bc },
            .Y = { 0x691478a411196, 0x22c6a8f5b56df, 0x2a6321814d453, 0x1272a42f91f9b, 0x50d6460185d89 },
        },
        {
            .X = { 0x1e64086f3cd53, 0x348b97ad2a847, 0x1a4c92d172080, 0x5e3a31057d882, 0x3f30ee695b236 },
            .Y = { 0x55dbc0478370, 0x9eada30f7501, 0x1823128308168, 0x72aaab207d757, 0x414231531dd52 },
        },
        {
            .X = { 0x278966b88fcca, 0x15f79b907161, 0x746d3bdfe36d9, 0x16164ba51661f, 0x4fbfdbab00559 },
            .Y = { 0x265662d061c93, 0x4e01fa312f480, 0x6ff073bb00eaf, 0x672d2a989fc1b, 0x68ffb955a83a0 },
        },
        {
            .X = { 0x5307d306de6e4, 0x57f9f30ef1846, 0x4bb6196df0d69, 0x5096e630652e3, 0xebc38c54fd6a },
            .Y = { 0x3f23886763ce2, 0x49eb1af02c58f, 0x282149b6f6856, 0x7ed9f18eeeab, 0x4b3a1d9f2ef4 },
        },
        {
            .X = { 0x185647241a6ac, 0x1dfa014c2f526, 0x4e2b92c1faa0c, 0x59a456a15db2b, 0x14c053b040dfe },
            .Y = { 0xeb9cd0eed3c0, 0x1f8ef9fc8fe85, 0x5673222a1e56e, 0x39bcbf3e94e33, 0x7ec77091bc605 },
        },
        {
            .X = { 0x743240d99fd8b, 0x9f4fac3d7fcd, 0x6acd80f4e86b9, 0x23e152fe96eb, 0x6518baedfba8e },
            .Y = { 0x674b4a4f04a46, 0x2239e6860b4f3, 0xca1d534c759a, 0x1d108fdfdb2be, 0x7388f7538996 },
        },
        {
            .X = { 0x5c77bb5d37d53, 0x18c108d565a62, 0x488bfbbe1f5a9, 0x3aca7117c6198, 0x78ad307ea1d91 },
            .Y = { 0x55e212791a2d, 0x2f1d7a2ea683c, 0x5011ef683123e, 0x1283fe95fe601, 0x601b077b6cb16 },
        },
        {
            .X = { 0x21058e60e49db, 0x546efb6bb4a4f, 0x731f39e284f9, 0x6c67dbeed9192, 0x3b75696730dc8 },
            .Y = { 0x46016166deaa9, 0x151888330ec1e, 0x5becfa2d, 0x4e7f7cfcf532a, 0x34d031744cd2d },
        },
    },
    {
        {
            .X = { 0x31eebdf40de4, 0x295b0c2597da2, 0x42f0cc8de40bf, 0x600d242cbf639, 0x71dd75fe35761 },
            .Y = { 0x1533e9f88a411, 0x3775efc03676d, 0x172d57e053ab6, 0x71a1bc333df57, 0x39674a4532078 },
        },
        {
            .X = { 0x71e0320587090, 0x638ab1379164c, 0x2320b7928b7ae, 0x730b85c0a6979, 0x300d6b0bd6734 },
            .Y = { 0x4a7bf5c55fc81, 0x531e97e1c57b8, 0x6f2af4fea65f7, 0x4863f8215c1e3, 0x40ca073d03431 },
        },
        {
            .X = { 0x3b7ed36905232, 0x4e227f866e886, 0x26e92fe50d60f, 0x6b46c251a6794, 0xceb59a95601d },
            .Y = { 0x3582b61615a85, 0x6f604bf141f32, 0x415753f2693e5, 0x676bc22136673, 0x1a781b6de1c34 },
        },
        {
            .X = { 0x181168cbec8f9, 0x6190b02fed2c7, 0x349140f0bed60, 0x6141335703b29, 0x42545a6a59885 },
            .Y = { 0x315672c2eb507, 0x341e3cd307f73, 0x2cb8182c4a75b, 0x5edecf5134dd, 0x706406cdbd1ce },
        },
        {
            .X = { 0x6cf68a8476ff2, 0x730dca3a6c583, 0x5996a3a3fe7e7, 0x143d193ccfb02, 0x60fba0618ab41 },
            .Y = { 0x7645080245d2f, 0x799d71734d711, 0x1b4ce449fd612, 0x7ff4231bd14d7, 0x50fee504978da },
        },
        {
            .X = { 0x78f89e18cfa41, 0x2abedf1e4b904, 0x1d4bdb6577be1, 0x7437ea227158b, 0x7e97763970e96 },
            .Y = { 0x7ca1146b6217, 0x5b4b15b0dcf3a, 0x513b0ff4a4e31, 0x747c600c8b746, 0xe85b4c08f01e },
        },
        {
            .X = { 0x52a0b25764cb8, 0x76bb5eda60567, 0x11ebd9a6336e8, 0x4fdb604eb7a63, 0x208622a2f72f },
            .Y = { 0x49cba531169f5, 0x6403c323153c3, 0x3ed1677f04f91, 0x682556c3bb6ee, 0x14467120e8a31 },
        },
        {
            .X = { 0x711e441703a02, 0x7ca9f9cd5846a, 0x27ddb2c8b5f35, 0x5a8bb39d06710, 0x6c1262e3f8a43 },
            .Y = { 0x6f0d269c007f, 0x2d8bda0274fe9, 0x5880aa13c7ec2, 0x145cd46a84513, 0x193841cf1e0f5 },
        },
    },
    {
        {
            .X = { 0x36b8ff4eda202, 0x7daa346bd67c1, 0x2822a5801e36d, 0x4eaea25b067da, 0x6222bd88bf2df },
            .Y = { 0x6fa5782e45313, 0x117520560d1e3, 0x6df13d5042d8, 0x12eeb5ed7693, 0x325bb42ea4ed },
        },
        {
            .X = { 0x2aa2d8bdba597, 0x337727e412228, 0x682a0453a101b, 0x262572fd31592, 0x23bc7abc84cb },
            .Y = { 0x511df0f29c9ee, 0x1e58c41b9ddb0, 0x5c81ba413e52e, 0x58a64a8101b8e, 0x4d2b97a739ece },
        },
        {
            .X = { 0x7640e33263467, 0x4d99d421b1000, 0x229877cb2a32a, 0x20e2cd45a4e5, 0x132a065edb5c4 },
            .Y = { 0x5fa3e1dd7de2f, 0x3863fd1d4b30c, 0x7973f46ce42c3, 0x3c03b16b5bdc2, 0x534ef70a3532f },
        },
        {
            .X = { 0x633fbbd39d169, 0x2de1deb9e1897, 0x1cb211e5ff1a1, 0x252cc055229db, 0x6d5066cf7137b },
            .Y = { 0x347115219a417, 0x68445d52b6b96, 0x2e0615fe54802, 0x3e441d5f1ad5f, 0x54bb8cd82a0a8 },
        },
        {
            .X = { 0x2429c7b04c2cd, 0x1f7334164ea0e, 0x2e82a3d8f00c4, 0x49a3fac10ad3a, 0x35ff8f7cdb086 },
            .Y = { 0x49cbaa6ce8b8a, 0x2fbfc351521f8, 0x6ed81ad386d5c, 0x782aa34f434a9, 0x50e1cc6871155 },
        },
        {
            .X = { 0x2120a622e0213, 0x4928e70b4255e, 0x20ca395004af1, 0x137b5a74383cd, 0x6c6365c17f4bb },
            .Y = { 0x1499e5494c782, 0x54c171a7d6061, 0x2a7b2382370cc, 0x4b4641905002b, 0x1c5703a9d3d1a },
        },
        {
            .X = { 0x44c675bd887d1, 0x309183c31e772, 0x1f3aa0d1e7794, 0x2965c1800a0df, 0x68b818a50b31d },
            .Y = { 0xe4dfed398826, 0x2a6b5bb8ee8e5, 0x5b63d0e5a1659, 0x35914773a6ba9, 0x1e8b233ec0076 },
        },
        {
            .X = { 0x3e8be859362a9, 0x285b6b601c94e, 0x277aacb4ea942, 0x6a71a039dbb31, 0x6b66159ac8702 },
            .Y = { 0x61a22c8ca96c5, 0x644c063cebfd, 0x110cae7398c22, 0x282724d9d2eac, 0x19dd4bef38efd },
        },
    },
    {
        {
            .X = { 0x3ce90b2f23e9f, 0x6c6fba067cc9b, 0x5a6d491d9c150, 0x2d93a15d07e01, 0x1f6a1fd8cc0e0 },
            .Y = { 0x865869605e19, 0x3859d2657a634, 0x2a19102ea1749, 0x4d4d98e077c3a, 0x779964401ee86 },
        },
        {
            .X = { 0x35cbbfd4f326c, 0x3ee294ccc91b7, 0x353e636b68de4, 0x502fd3833e7f6, 0x6f0a87af6e0f4 },
            .Y = { 0x1248c6c6e36c6, 0x3c99a44d7cc01, 0x21f30757f285d, 0x13ee5e5b52174, 0x6b60138135059 },
        },
        {
            .X = { 0x6af0a0fcd15f4, 0x5d8829faa2d69, 0x59b2d11d2184c, 0x6192b3a4edb35, 0x398c8c8c8e8da },
            .Y = { 0x5ef111a7459ca, 0x743e76b873eed, 0x7da788d05038f, 0x132d6dfd110e8, 0x7fc5e88a596b4 },
        },
        {
            .X = { 0x3134a847a0564, 0x26d734dd61ed8, 0x1d5d36768c9ad, 0x6841d9eb25d62, 0x1f90c7ef4610d },
            .Y = { 0x1a1c8ce3a2f7b, 0x6c5088246078f, 0x25f03d3f7fef3, 0x55cbd09e1e658, 0x63344ccbe4169 },
        },
        {
            .X = { 0x59fd686603f08, 0x5ae148dbd3c5e, 0x768558f36db1a, 0x48da4f4ae0be3, 0x3d22b58c663ea },
            .Y = { 0x2b63fb03b0eb2, 0x73e96f8ef32e8, 0x45daaf6a02597, 0x39399ec5ff106, 0x7240c7e82ba83 },
        },
        {
            .X = { 0x45e86f1900608, 0x1fbe96f3948ec, 0x6f302c3bdfd3d, 0x799f280c2de8d, 0x6e6c09db77eed },
            .Y = { 0xeceb9c916f07, 0x49ea727e1b83c, 0x6273e2eeb6666, 0x171f72b4fc831, 0x5640b667ac2d7 },
        },
        {
            .X = { 0x9339d2475746, 0x64fafff9a96a0, 0x64bc3ddbb71e1, 0x401aa64372298, 0xa8728e5d7851 },
            .Y = { 0x7d41cded490c8, 0x612647ee1199d, 0x4195596834dfe, 0x38151f531081c, 0x7ef39552f5076 },
        },
        {
            .X = { 0x633092dd34e07, 0x49ce7d7c1ba41, 0x2b7a22c02deab, 0xd4941a46bd55, 0x47ac42e7c5948 },
            .Y = { 0x9bb58f7a8924, 0x6a3da0c80785e, 0x6ba5d3ceca93, 0x75f50ae5fe8b, 0x28a6c1d1356c1 },
        },
    },
    {
        {
            .X = { 0x1d2b7be4610f4, 0x7af44ecbd9da, 0xf3d96a079370, 0x29a10cebd576f, 0x608de273a9f6a },
            .Y = { 0x6d841f67144fa, 0x1f09d66e26b18, 0x5eaa723731e58, 0x40d589897d941, 0x3b31d0113f98d },
        },
        {
            .X = { 0x3a3011634d581, 0x6f27d928a5d52, 0x2d7e560b44606, 0x31f977b27d505, 0x8d1e75a33bf3 },
            .Y = { 0x7b5ccae42cf1a, 0x4da0b6b78872e, 0x449c557409d64, 0x520f54e1ec44e, 0x47dd939350f06 },
        },
        {
            .X = { 0x43f0dfd1da83e, 0x4113b0ddf4e1, 0x53381c4548146, 0x6514dd84675fb, 0x4b55df555ef78 },
            .Y = { 0x66e8cdcd6194d, 0x5ab028755a609, 0x3cb9a10526f5, 0x5a95174bf6017, 0x2faebe97416b3 },
        },
        {
            .X = { 0x5aa1c6d51c96f, 0x5dc4287f214be, 0x13b82b3f59239, 0x50514de60f291, 0xb2fcb60a19e6 },
            .Y = { 0x144a1f33239dc, 0xf2bf6f1878bd, 0x77897a0d0d4d1, 0x1c336bb5a6349, 0x6301b568e9c61 },
        },
        {
            .X = { 0x28dd62ec76e, 0xb1d7e690987, 0x6c62c01be597e, 0x38a7726f51b21, 0x6d187e11aedfd },
            .Y = { 0x464f0c1ce745, 0x2ac3a1782fc5c, 0x4f978c41c144b, 0x620513ea91bfc, 0x3607198a1819d },
        },
        {
            .X = { 0x4d99764529a1f, 0x553a4cdea2103, 0x3a5dfe90cce4e, 0x475ba8c2ecdb1, 0x4db4418f87f77 },
            .Y = { 0x513e93e82a813, 0x1bb59f9403d75, 0x7b71eb1db3df, 0x540312aa76437, 0x5140b9860c78e },
        },
        {
            .X = { 0x31dda40465406, 0x650beb1c1a888, 0x123a5c9259b42, 0x6c6ccb8df73b6, 0x6e1a9bb685ada },
            .Y = { 0x39e6aca5e83e8, 0x6a3e7c5796320, 0x5007541e92105, 0x546cf963e1f5f, 0x31ef82c7b281a },
        },
        {
            .X = { 0x48f4c0dd3ae27, 0x1f15ca78fbd46, 0x6d058c362496f, 0x66413f5ffbad1, 0xb216ceec26c5 },
            .Y = { 0x7aa2a5312bac3, 0x5d14aaf0335ae, 0x4e245bb0a1d80, 0xc79262bd1fc8, 0x563053b0dec55 },
        },
    },
    {
        {
            .X = { 0x62dfd0af81ce5, 0x5682fae1ebeaf, 0x5574666fbf845, 0x2b64be05f9894, 0x69d98b5ecc35d },
            .Y = { 0x481f8259f4e8e, 0x3702a9aa1c5aa, 0x23fa52f3a4a9a, 0x355a270651d92, 0x3e26d487fbfe3 },
        },
        {
            .X = { 0x76ad49dd4b6d9, 0x7b3c20580ee15, 0x1c0cc9b92b446, 0x3acfba0e3c49, 0x62a8a839f4566 },
            .Y = { 0x48b613555bf3b, 0x1182aa75d12e8, 0x18044a657f4ef, 0x1e14e88e58f4b, 0x3d45d6c61b8a0 },
        },
        {
            .X = { 0x5144ba1ee3347, 0x338d704ba14ab, 0x790cd10cc448a, 0x4d146bf59d4bf, 0x5ff031a9e8f4a },
            .Y = { 0x27f6f65c163f9, 0x6aedfe81ae652, 0x9bae0494b6b8, 0x5582075728af7, 0x67c7ce2874df5 },
        },
        {
            .X = { 0x30dccb2500e7e, 0x7bda84b6e34cd, 0x6884bf3c2b2be, 0x1f988f46d271f, 0x4e1c036204e12 },
            .Y = { 0x561736f1bb1c8, 0x3e4af4fa1a4e7, 0x93da4cbc39cd, 0x53151564827ec, 0x6a5908d961f00 },
        },
        {
            .X = { 0x2db65174136b3, 0x781add46ba081, 0xa50b0787eacd, 0x238301afa0ea1, 0x2ec3414992964 },
            .Y = { 0x33c31999aba40, 0x7ba81c7c73674, 0x60583c9cb93f0, 0x51662ae1e2a53, 0x2ecb17530e69e },
        },
        {
            .X = { 0x2520ab77d56f3, 0xaa1c5c41aa31, 0xa4e157873e5b, 0x226534ec05356, 0x1216d39000ebe },
            .Y = { 0xf4fa151fd1d4, 0x334c93c7047cf, 0xc93b40ed2f43, 0x1deb821d55c89, 0x374132be82d97 },
        },
        {
            .X = { 0x7f0362dedb022, 0x4f58fd7218c8c, 0x4160a0bb8284a, 0x3f301cb4c06e8, 0x3cbe5d79f358c },
            .Y = { 0x6dd1a92e82034, 0xc1d093c5ed2d, 0x2f9b316b3b557, 0x1731c87ed1911, 0x2aaa635d3e22c },
        },
        {
            .X = { 0x8e2ff8552d6f, 0x644dbbd47631e, 0x79ae397fbd745, 0x79d24a3240869, 0x7fe8906b455e9 },
            .Y = { 0xb97b336976d3, 0x3ad6534ddce08, 0x128fb06357743, 0x2818101d89bf5, 0x1300ff9d28f13 },
        },
    },
    {
        {
            .X = { 0x45ceb3b5775d0, 0x71709f92a0ac2, 0x2a7d6afd28c7a, 0x64b6d414a0a90, 0x51f4ff8c599b1 },
            .Y = { 0x406b0d7863ac1, 0x700fa97a36ff0, 0x14bbad2c4ba01, 0xf9fb7ccd771c, 0x35ac9588d46e4 },
        },
        {
            .X = { 0x29b6a71bf9741, 0x1dccd7f1e7d8e, 0x149a0be95a5b0, 0x31fb1e040b0ec, 0x568de690133ca },
            .Y = { 0x5b123773039e8, 0x7e9ae87c7a37f, 0x3996d0e96eb9, 0x323a0ee6e219d, 0x4c48220992e8e },
        },
        {
            .X = { 0x206c8b59f5785, 0x4df5e07e8f3f6, 0x5f659be6ec6a7, 0x3193a84b2d27c, 0x4f2f4b477f6ca },
            .Y = { 0x269d9359e934, 0x4b847999e4a90, 0x4118b05ab8e14, 0x21cd0b6990453, 0x13115f873b8a8 },
        },
        {
            .X = { 0x30cc4663403da, 0x61f36a4978dce, 0x48df3d3a54a05, 0x10ef7a35c4fee, 0x3342c4717d552 },
            .Y = { 0x5c6a85b6937c5, 0xcd0dcc3f9493, 0x1f7499c735fa9, 0x29b46aa6678fd, 0x50676cdf00c93 },
        },
        {
            .X = { 0x30fb8f1eccccb, 0x283e91bedd33f, 0x2235667e8bf41, 0x7685498387edd, 0x282a400973b64 },
            .Y = { 0x62dd27aa84686, 0x23c1862a18fea, 0x31aff366b3a83, 0x549acae4ff00d, 0x5a8a95d8ceddb },
        },
        {
            .X = { 0x29d9ed297dbd3, 0x1d030c6eca418, 0x7c356254aecce, 0x106a1f2de6054, 0x7aaf48f59f15c },
            .Y = { 0x1896bf1a58d6e, 0x11d1c1269b58d, 0x798b78e2a3c2b, 0x1c61b0a4e31df, 0x2765ba6a9e766 },
        },
        {
            .X = { 0x5979baa2329b4, 0x19a93ef8737a9, 0x46e5644cdd17d, 0x1aae72fc62ac4, 0x336498fbd2b4 },
            .Y = { 0x8a7c5dd5cbd8, 0x25ffe91b3543c, 0x26c427ebf83f5, 0x673f2258e1bcb, 0x653338b616279 },
        },
        {
            .X = { 0x3a45d29424d9a, 0x77229b1550ded, 0x48807bcdb7f49, 0x19eac75c6e662, 0x3396978bfc50b },
            .Y = { 0xca4ac8073393, 0x794fbb7b5763a, 0x41f332d14fab1, 0x13ee5ce9adfc8, 0x5ccf1359113c1 },
        },
    },
    {
        {
            .X = { 0x748aa01d5a90f, 0x79743464cc509, 0x26c605e0047a9, 0x609348312c42b, 0x1924416956ea2 },
            .Y = { 0x1fb8adaf021de, 0x3304d5919b716, 0x3ae233f1f6dcc, 0x6e9037db7f74b, 0xc8e28127b7dc },
        },
        {
            .X = { 0x2ff28ce974493, 0x6937edeb88807, 0x36263ee101daf, 0x6a4f62b81c676, 0x2898ffaab663e },
            .Y = { 0x3bc46f335163d, 0x69f49f6d8de96, 0x64d155f984e83, 0x23bf3aceb5e5c, 0x94f3b96ab6c9 },
        },
        {
            .X = { 0x6d2e98c19a76a, 0x7097054a99d7f, 0x49d1cf2973b62, 0x3d7b6d0bb4e8d, 0x59de4b2305d8b },
            .Y = { 0x7e2f9f1dbe4d5, 0x4b382e4dd288e, 0x7b29b90c5af84, 0x4c227f0d32316, 0x4078b0875ce14 },
        },
        {
            .X = { 0x14f12016bf75a, 0x648f2908e182a, 0x5f335dc5b006, 0x882eaeac87b3, 0x34e2ddf511570 },
            .Y = { 0x3ac585c1fd926, 0x741f0b87a4071, 0x3361a39b5ff0c, 0x38bbbaac7ba29, 0x1bdaea88b78eb },
        },
        {
            .X = { 0x1b303eaac5b05, 0x4d791447cb32f, 0x58b15e1815afe, 0x4a7e15c91a795, 0x55eaf5fea101c },
            .Y = { 0x632fc1e0e6d82, 0x565caf3ff6523, 0x213481073d8a9, 0x63f189534493a, 0x644f4f10c2a13 },
        },
        {
            .X = { 0x12093400eeab6, 0xeb508c2d46af, 0x1a7b5bddfe5b4, 0x5f8663b12868e, 0x5726e2adc5bbd },
            .Y = { 0x57d966b0ee8cf, 0x21b35279a24fd, 0x3be197a1262a5, 0x782c4877c786b, 0x79735f155fd92 },
        },
        {
            .X = { 0x7c6b674fd1963, 0x3d72f92ba5324, 0x2606b6ff7387d, 0x7d0a2401f886f, 0x2fab7584f9eba },
            .Y = { 0x1da2698c7f981, 0x531652ca579eb, 0x2c8dd914863b9, 0x2246d7a27ad58, 0x27b70248f415f },
        },
        {
            .X = { 0x69fedfe165cc8, 0x154f3e97ecac4, 0x3761e3297fa14, 0x12aac61a7674c, 0x4cb56bd83d7bb },
            .Y = { 0x4a7fa15a03865, 0x61370d89d71f6, 0x66a2ba9357a94, 0x48a29adec0d4f, 0x5d1f6fb463251 },
        },
    },
    {
        {
            .X = { 0x1d364fc3a757b, 0x26b33493bafc5, 0x416cd4a9ed2fd, 0x1428d0b57e1d5, 0x4339fa86a2324 },
            .Y = { 0x5fe8a63a34b0e, 0x553b7a27e2b14, 0x44205c101d9fe, 0x48b05df5dccba, 0x6d07cc3462bae },
        },
        {
            .X = { 0x64e9736c701eb, 0x4f74581abf576, 0x29bfef3795591, 0x72b1cc6e98276, 0x62332ee4f8ee5 },
            .Y = { 0x74bc89fb8ef65, 0x6b25369243fb4, 0x5e7ca6c2b5a08, 0x7cfbf3517a3f8, 0x3e1bfb3c0218c },
        },
        {
            .X = { 0x2396aef810769, 0x3272ba4e64da3, 0x5df47a5a5db18, 0x6e6a42ff580c8, 0x33287cd3c57f3 },
            .Y = { 0x46fea1c371cde, 0x1eac66533d582, 0x4b3dd317dc0d6, 0x44d98937f95fb, 0x70f3e2be18824 },
        },
        {
            .X = { 0x4d15140c88b2f, 0x54553c816355, 0x6fe5decbc6485, 0x7a79e9fb6abfb, 0x7b9e0081b2e20 },
            .Y = { 0x3f1fcc42b858c, 0x2f098844f3d15, 0x7e5f69a1be8e8, 0x4a6902b81b7f9, 0x93854716ef97 },
        },
        {
            .X = { 0x64d47a7c2e55b, 0x3c06252c62301, 0x3531bc0abc846, 0x57ac3e1e537e1, 0x1c70927aa306e },
            .Y = { 0x5654a077444f0, 0x24dcdd6382196, 0x6d36009a3a1c3, 0x19f2ae05d7a61, 0x2e259ec780da0 },
        },
        {
            .X = { 0x14989f13c72ec, 0x3937f6987889c, 0x30225657ad7fe, 0x7541db44db135, 0x1e22f8e8551ea },
            .Y = { 0x117e4d3609883, 0x665de1adb5d80, 0x6ab19361681c7, 0x4786c1529cc3, 0x121fd5fd0c3ab },
        },
        {
            .X = { 0x62a41118f91a, 0xe4448cc3464c, 0x5aab3a49c1610, 0x3c20812138b8f, 0x6237e93647e5e },
            .Y = { 0x5165245f5b2fb, 0x560ada6d8f2eb, 0xc39412f9561c, 0x7862aec0bca41, 0x438c3caf8e404 },
        },
        {
            .X = { 0x15e2fa8742940, 0x7b727de748f3f, 0x221a66ab0c4cf, 0x148f99690bb8f, 0x7ecfa6b215265 },
            .Y = { 0x37da46c4720c6, 0x7dbe06b7d4c79, 0x1081f39b60620, 0x57f2abdbf300c, 0x10958b5cdaf36 },
        },
    },
    {
        {
            .X = { 0x71359b6e6424, 0x6c664206679cf, 0x22c56e66dcfc4, 0x6a7f6a6e7b496, 0x4da4821314574 },
            .Y = { 0x70759a40a6e65, 0x6cdc3e94d7e42, 0x36adb918af2d7, 0x1690294de49d1, 0x314016034673d },
        },
        {
            .X = { 0x5a1bd156d734c, 0x300c481626b89, 0x607fad6c71e2, 0x4ed332aeca1aa, 0x22423cb8e8212 },
            .Y = { 0x223156f08cdb4, 0x4fe3a3da44163, 0x33a7bcd15cf1e, 0x4c75b1cfb5bbb, 0x627dcf9be9be7 },
        },
        {
            .X = { 0x1bafd72fc5b92, 0x43c72af94dfbe, 0x350af500fa4fd, 0x29f1926edec90, 0x32beb4e36411f },
            .Y = { 0x1fc9e7901786, 0x77a0725ad54a2, 0x3684d67a69b10, 0x4281e5cd0730d, 0x425a36157c40b },
        },
        {
            .X = { 0x104dcab320bb4, 0x32ea163c42aaa, 0xae44e8d5ce26, 0x4e0bb7524f3ff, 0x4230253b946f },
            .Y = { 0x558741d1e5afc, 0x2e7d2ef71f4d2, 0x7bd5e98b5e4ec, 0x1cf3726858122, 0x4db9dd92f8d9 },
        },
        {
            .X = { 0x33c62b35f9f4f, 0x763ca1ca9ea10, 0x7328952fd0e0b, 0x336f7f1a11b08, 0x65f711807e588 },
            .Y = { 0x616d7f9d9637a, 0x27a164e67fa6a, 0x52357e95fd6b, 0x69112a118ad5f, 0x66f79ccc68c4 },
        },
        {
            .X = { 0x2a4052f89d69a, 0x649357afed0a3, 0x458a2dac848a9, 0x123f4425ee350, 0x115b217ea5058 },
            .Y = { 0x4137e9279f1a, 0x4f53cd9217a50, 0x76434fad97c1b, 0x3a3cde89779be, 0xd6e86f3a086b },
        },
        {
            .X = { 0x4a0b1d181a9aa, 0x2b698a3a2fda3, 0x7f3967193c8c, 0x22ae5f667f08d, 0x1e90739a6bbf8 },
            .Y = { 0x5f82287b7869c, 0x5566279a9314, 0x35a7f8324bc4a, 0x165289fe924ec, 0x43299d9738472 },
        },
        {
            .X = { 0x691589756d7c0, 0x60c77d3e9e13b, 0x782f1750e1a0e, 0x54745dfbae079, 0x63b06e648bf7a },
            .Y = { 0x249be08bae16, 0x6e11a18488b8d, 0x75361fff589ee, 0xdf8d967ea85b, 0x1b17d88ed9954 },
        },
    },
    {
        {
            .X = { 0x47ae60b7e824, 0x1385ce47cbf90, 0x538a682639a17, 0x1964a969cc270, 0x4c27afff3c45f },
            .Y = { 0x2bd114bf5a66b, 0x3ca349893cb77, 0x30a70ea4342f8, 0x43ecaf88f5b13, 0x5f2c99e6526dc },
        },
        {
            .X = { 0x7d1b43224e085, 0x651f7f44d3f9d, 0x1f5bb93da54b1, 0x57bd040abfbc8, 0x786be30733efd },
            .Y = { 0x30712c63e2736, 0x7d673ad37c9d5, 0x3f4211ca9f022, 0x42d9a138766ea, 0x653a5f772f349 },
        },
        {
            .X = { 0x5ca89f193c7c7, 0x190eadb296624, 0x613c26eba92eb, 0x28e517e9d52c5, 0x9c186afb8339 },
            .Y = { 0x49a357f7b062f, 0x577daa9fe2346, 0x61928780aa0a9, 0x1a9c9a34ad8a4, 0x6137b0746027b },
        },
        {
            .X = { 0x22f04c2eaa13, 0x57d69a3366d97, 0x72376731a9341, 0x499efc4abc0ad, 0x21fee4804968a },
            .Y = { 0xd3e930901700, 0x7512e5846260c, 0x2160ce6f694d9, 0x28ea0b62ed0a8, 0x500b7740072cb },
        },
        {
            .X = { 0x1e3a2b12d4f17, 0x4b2e7932aa923, 0x22727a3b68433, 0x415c09f01b2e, 0x4a2e1f96eee4e },
            .Y = { 0x4b832ac846fc4, 0x22e66b4cc889a, 0x77c36a3708a79, 0x62dc64a88c45f, 0x6c3f24822e185 },
        },
        {
            .X = { 0x1449ec0ec3464, 0x54da0a6d415e8, 0x27490d51894c3, 0x3a33578cadec3, 0x5dbd3bf95494d },
            .Y = { 0x4e1c737e25b77, 0x54f6f73f1826f, 0x264aae68d0b38, 0x5d8431e6c6054, 0x56f5f77776e9f },
        },
        {
            .X = { 0xbadc76ed5685, 0x35fa4ebc2326e, 0x5dc73aed63804, 0x1e078f96abefc, 0x4b3702044575d },
            .Y = { 0x27acd289b820, 0x1f5f99c524904, 0x581aabf8db72c, 0x17a97a13d4072, 0x54333c50acf33 },
        },
        {
            .X = { 0x7136e1146b3df, 0x59e3baac9c516, 0x25223e30d62b6, 0x5b57250cc032f, 0x77fe8a5d490af },
            .Y = { 0x5c5afa5f50246, 0x7c146a8b74dae, 0x48636448ab327, 0x18b45600199ca, 0x3c530f01e039f },
        },
    },
    {
        {
            .X = { 0x7fd3a6196e3d6, 0x284949203f37d, 0x476c5437b216c, 0x706c3c52af457, 0x7b8d2c823baaf },
            .Y = { 0x14483f8af0c85, 0x3291ef9e8bb29, 0x7868197c6d14f, 0x7cdc4f68ad9e1, 0x1c45a557b3624 },
        },
        {
            .X = { 0x51c3cd099c188, 0x73201e27df68b, 0x3731a5654bcda, 0x679c6f128856f, 0x5fa2b1dfec51b },
            .Y = { 0x9b7d23a1682e, 0x217e84f70ad28, 0x14b5cbd03592e, 0x45eb26f10d929, 0x3f81faa56dc38 },
        },
        {
            .X = { 0x2c70bc7009528, 0x63ed4991c551, 0x1696a5aa08d84, 0x64cd2d131eacb, 0x3780a1d4b6b5a },
            .Y = { 0x15fea238825fe, 0x2cca59dedb077, 0x3bcee6378c63, 0x2c65b7e4f9b48, 0x3aa8dc131e632 },
        },
        {
            .X = { 0x6668a7b47fdfb, 0x6fdf04c85cf33, 0x4261b63dbb087, 0x2c3501eebe18, 0x1f878c9bb777a },
            .Y = { 0x1553da8e44755, 0x69c15c3b56684, 0x2fd62e7176fab, 0x6fa0d18c49562, 0x59479937cae12 },
        },
        {
            .X = { 0x5571a8e1c47ff, 0x33ab17ab47cbe, 0x74678e2fb0c24, 0x52b1f350c7eae, 0x58458308ef6c0 },
            .Y = { 0x626fc2bb1c02e, 0x64d7ddcd0c1fa, 0x8c25c9d513a0, 0x5bc4b24a96867, 0x259d664d2b336 },
        },
        {
            .X = { 0x3ff13e0665cb0, 0x294f79a7ca0ec, 0x401440d01f5e6, 0xafc7b8ce2b99, 0x333f8eef2b1f7 },
            .Y = { 0x7f4107b4c7b5a, 0x650ff8676ce10, 0x7b888c25eca86, 0x7d51b6b254d8c, 0x131883d8fc4ca },
        },
        {
            .X = { 0x622afd443ec39, 0x423d5404492f3, 0x4239477208a1b, 0x251f6c0eb46ad, 0x7b10e1d26d95f },
            .Y = { 0x6817ca6919549, 0x1a4913e350c0b, 0x40ac9fda7c686, 0x19ee4a0633fec, 0x36663875e8dd1 },
        },
        {
            .X = { 0x5a949c163cac7, 0x64c9b4fcaa8a6, 0x77ab0cbdd401c, 0x79d5908cdac3a, 0x8e60479f7f51 },
            .Y = { 0x6e1fa4b22fa46, 0x2ecf74cff92df, 0x17e42186f9f12, 0x6e6d908adfa77, 0x1b2b6c48c2eea },
        },
    },
    {
        {
            .X = { 0x778185eed3f91, 0x89181a58467, 0x3475a3ba7efa0, 0x625d74f2bac1b, 0x2b556bbc070b5 },
            .Y = { 0x184e1d7174d6f, 0x665dfb2162f1b, 0x303e86cfc66d6, 0x6c497006ed9b4, 0x7ba01332807eb },
        },
        {
            .X = { 0x4444af406f1a, 0x83da01b1e876, 0x707fa3974642f, 0x7027c48ae62f9, 0x3f5c47b490be4 },
            .Y = { 0x132fef32c3bb0, 0x408b75547e0ee, 0xe72d53aa36a1, 0x10a6945f95a08, 0x537ce98e07428 },
        },
        {
            .X = { 0x18fc98dc1aeb0, 0x40f375deeef57, 0x4ff89abd6a0f2, 0x59222a5f1802e, 0x1e2b1d5578437 },
            .Y = { 0x5a06716d6b801, 0x634f48b1c3722, 0x27c4a2cbddcdc, 0x469331b639ffa, 0x41c6747ff756a },
        },
        {
            .X = { 0x4dd30a8b4f85d, 0x23b195a74a719, 0x5cb5244c6cbe3, 0x325411da52b82, 0x614118e22068c },
            .Y = { 0x51229478dd419, 0x176a8ef02360c, 0x5521417ea968f, 0x76115c6cb99d4, 0x63ebe5283599f },
        },
        {
            .X = { 0x12e2aef773c8a, 0x1842ec3c8d6fd, 0x3cc4861ad5071, 0x1d842a007882b, 0x233475488522f },
            .Y = { 0x56b97a812b288, 0x4b7bc1f58e553, 0x7597adf006ac4, 0x38a9f01dca3ca, 0x37e8d773be2a4 },
        },
        {
            .X = { 0x7037bcd22844c, 0x471b3dc4be217, 0x6a69cf343bc94, 0x696cd252f3afc, 0x318ac05007ad4 },
            .Y = { 0x3aac8823222a2, 0x1a3859eafe3aa, 0x766da90b03c67, 0x4139a3a954bbd, 0x6bf5a299dfb3f },
        },
        {
            .X = { 0x3fccb0d152c96, 0x992ba098058e, 0x521be0be8427d, 0x4d5af7f7c5351, 0x1b76bf6bdb257 },
            .Y = { 0x6761efd0c3948, 0x5e84e4ea6cae0, 0x7c0fbb51669a1, 0x718798b9f1069, 0x502f67ac28825 },
        },
        {
            .X = { 0x4f4d8ef3f3fb1, 0x560ca0c14167f, 0x902c55c148a4, 0x7e60c3caffc12, 0x13aea1849989f },
            .Y = { 0xc101c2b81f44, 0x4820c00aaa326, 0x49795c19cb5b2, 0x5b87deedff90d, 0x884949c8ca12 },
        },
    },
    {
        {
            .X = { 0x4341a5f3da1a, 0xdd4e3cefdf85, 0x1034cbb3164ef, 0x1e6a23789d9f6, 0x1cd96ef237b21 },
            .Y = { 0x4a5ccd60dbe3, 0x18e26ce7de1e3, 0x5f3b62d047da, 0x5afcbbb9327af, 0x7a626aeb91efc },
        },
        {
            .X = { 0x5810897a24a18, 0x7d07e19989065, 0x3ed6d325437b1, 0x62b3325404c7d, 0x275d04f2b3510 },
            .Y = { 0x62e5a06b8b93b, 0x2347386f0587f, 0x5218f6a9a707c, 0x75e5316ef56cd, 0x57dd6527da326 },
        },
        {
            .X = { 0x5a501bb5309c5, 0x1a75986e32bc5, 0x7614d69097315, 0x45c3cca33a9b8, 0x5dc17a1192ddf },
            .Y = { 0x6f21cad653d78, 0x385731096c7d0, 0x487b74578e2b3, 0x90717cbfe980, 0x68538e31bee79 },
        },
        {
            .X = { 0x42e7d31374f84, 0x6ed6540f0f5b7, 0xd5d53bad7ff0, 0x39d0c8555ef53, 0x3c3b6a92efb1b },
            .Y = { 0x460396d94b23b, 0x6c763503cfdd5, 0x514aa5015721d, 0x583d89174c05c, 0x126d49709e40c },
        },
        {
            .X = { 0x3883000762536, 0x58d8af1db21fe, 0x40c920c3a655c, 0x2cd4c1a16f7fd, 0x4962227254243 },
            .Y = { 0x41925fd7ce64f, 0x4393bf3f965fb, 0x63f4f071176fd, 0x6730d561b7734, 0xb370634de2b0 },
        },
        {
            .X = { 0x54ca8847818fd, 0x62e9cb4cb2dcf, 0x70544ba129867, 0x1aeb58a790994, 0x2201bfeff7515 },
            .Y = { 0x2fde64c624ba4, 0x7be5f903e40ee, 0x6e8a9d9a1f04a, 0x4050b8fd8b2fa, 0x2af4f8c0bfb03 },
        },
        {
            .X = { 0x69e8a4550bbb3, 0x55bc5555266d1, 0x9571894ea475, 0x3285919343bbf, 0x13e4b9507e542 },
            .Y = { 0x2d4bb274a03d8, 0x22d5abfbda6db, 0x2a96fb7475a56, 0x3853cbd45cda5, 0x79cf2e8f8e6b9 },
        },
        {
            .X = { 0x11de45b556055, 0x4ce13eb73a98e, 0xcb8a80b26a16, 0x583d2954f6724, 0x3cde77866cc73 },
            .Y = { 0x7437e967a18f7, 0x656c9df8ab52a, 0x74d4b5f93c801, 0x263b8fb42db69, 0x79f2b3d77f3f4 },
        },
    },
    {
        {
            .X = { 0x3945652014031, 0x31dca5551450b, 0x2aad73634142b, 0x5fbc3dbd51c17, 0x543d84cb04fb2 },
            .Y = { 0x32ba4de59ef20, 0x293485d85cdd2, 0xac9d611bd0b7, 0x24f349ec1c78c, 0x358fdc5b63ed },
        },
        {
            .X = { 0xe1fb4f605ea1, 0x68129eaa3ed4e, 0x60718d8ce56, 0x25a521392830c, 0x2ac4c0386a42 },
            .Y = { 0x74860d7de3877, 0x4aa9ff50e9e00, 0x487eddffd0cc3, 0x2f23583d97f83, 0x1a36c19fc30e },
        },
        {
            .X = { 0x3a6db62a039e9, 0x7947312005e76, 0xb93d668de230, 0x74f0a829607a2, 0x2d2dd667a240f },
            .Y = { 0x791506554dd1, 0x22b97071b6edd, 0x6fc323592bbf4, 0x42913dd36a31c, 0x377bd9db79c5d },
        },
        {
            .X = { 0x283d3ac2cc513, 0x14a0bee52f90, 0x75bd156187ed8, 0x5cdb2e84c8617, 0x2816f8430d466 },
            .Y = { 0x1b17e74b47f1e, 0xcc956b4309f1, 0x319ab821d177c, 0x51c1a92648778, 0x5452c4b45430e },
        },
        {
            .X = { 0x46a6fba4bbef3, 0x7f7379b4035b8, 0x582fec791fd1e, 0x2d67d8ac3c282, 0x1caeba6f06a2c },
            .Y = { 0x56a7be98d85b8, 0x18d77ab3a72c, 0x977146a47b06, 0x2bba507e77aa1, 0x65fcdc11e7ea7 },
        },
        {
            .X = { 0xf5d51935a5c7, 0x4a699191958a, 0x374259f1ae7a4, 0x77a6462756390, 0x60c2952bf5ad },
            .Y = { 0x4b4205427f5b, 0x636039548e695, 0x7ef77cacb315, 0x124bbe329edd8, 0x10ef5d4944825 },
        },
        {
            .X = { 0x537b4701f0c00, 0x22b2dd12588b4, 0x1d947e640b8d6, 0x4aa8b50afee60, 0x5d41698923f4e },
            .Y = { 0x64780eef8bc11, 0x64fba820ef8a6, 0x60a63f7922bb8, 0x1745f6d6f5785, 0x2d92794a28e9f },
        },
        {
            .X = { 0x489c13fdc9fe9, 0x7f2183c84ee88, 0x7d337b8f29d2b, 0x40b31bfca128e, 0xcc495fe64bc4 },
            .Y = { 0x606036e75198b, 0x27c71834ded4b, 0x50deae783c8a3, 0x3bf7f6069f334, 0x42138e7b6cab9 },
        },
    },
    {
        {
            .X = { 0x4570a141e648a, 0x1803372cb9b5a, 0x423f8c22efd9d, 0x84ebd46a86ed, 0x7efb45d3a2851 },
            .Y = { 0x18acd0c28cda, 0x3b4643a86ca5d, 0x5c7e2dde4895f, 0x7ab433848e34e, 0xfe1198b11a1b },
        },
        {
            .X = { 0x6918f2c983218, 0x79e7d519e0255, 0x6cbb5a79176e4, 0x6290b675e8c23, 0x5d5225a5e2235 },
            .Y = { 0x6bc4e5de7b1b9, 0x3304ef028177d, 0x56eafbb1ad6ac, 0x6c69a454931b5, 0x7ec9e5494feec },
        },
        {
            .X = { 0x2c3cd228bac21, 0x75bd7af0bdd33, 0x42505d6feaeb6, 0x7d8493ed22c62, 0x6db3c0d1a1f27 },
            .Y = { 0x6ba7775d04f62, 0x5f25b171aeeee, 0x4713b58f4d261, 0x6d9f89747f807, 0x7b499ecb9b1b },
        },
        {
            .X = { 0x31d729a2e5760, 0x11184866b0dcd, 0x621aa263918e5, 0x304995ab111a8, 0x6fde98f557e33 },
            .Y = { 0x3f94a48002c8b, 0x22b7caa4d30eb, 0x27380949796c7, 0x55cf42209e1, 0x6994510fbdad7 },
        },
        {
            .X = { 0x7c406ee8b4d5, 0x63523e0ec80f6, 0x351753f66f657, 0x7defd1a2bfc27, 0x73d4d9f5b9708 },
            .Y = { 0x7c7fcc669e516, 0x566599c6e6d42, 0x340dc5ac83e94, 0x705432ef8d351, 0x256e14cee7d41 },
        },
        {
            .X = { 0x743a6a211a8a9, 0x2642651606a17, 0x6efee68a8553b, 0x30981d95b51e5, 0x5c3370c184b6f },
            .Y = { 0x6d7269a0ad07e, 0x3272066270fce, 0x446ac441625b7, 0x2002e9ad522ee, 0x292a52d9e9332 },
        },
        {
            .X = { 0x709f014389c99, 0x6e87f6da81f17, 0x68bd2434ed7c8, 0x55b78a638645e, 0xf9df8eef9562 },
            .Y = { 0x2674eb33be730, 0xe86052ddbb3, 0x3d3210ad427b9, 0x7f21ba8dbad29, 0x42ddc05b73970 },
        },
        {
            .X = { 0x42d25a9abdca2, 0x7dce106606bf5, 0x4e96cf0da57f9, 0x416ca8e4102e9, 0x705bd9027e77d },
            .Y = { 0x6cd670c1bfec2, 0x73c0591ca3c1a, 0x6b529073fc221, 0x1d0ca65236a11, 0x1417fa0628ebc },
        },
    },
    {
        {
            .X = { 0x6109a9afceb6a, 0x3ad4783875b79, 0x6b12516f11baa, 0x7fbf83e0920f1, 0x4892cfff7f94 },
            .Y = { 0x6ca461a543130, 0x178387c5978dc, 0x68fe3bafa05d6, 0x9ad3d1928e08, 0x3ae2aa25e2555 },
        },
        {
            .X = { 0x6098a9f1048b4, 0x6ee160f5e1f4e, 0x98ae114a020f, 0x458dd27aee251, 0x60c497780d40c },
            .Y = { 0x40c7086cb1d4, 0x3aede616bee67, 0x41d5d5cbe96c9, 0x10427b6510476, 0x22fa44ee0eb50 },
        },
        {
            .X = { 0x14d528a5f7598, 0x5d859b80803a1, 0x8cbc81e7f544, 0x1dacfa9ef2851, 0x7d7225dafd9c4 },
            .Y = { 0x5b18d4eebfb4e, 0x45fe5df3dbc33, 0x431f1a5dccdbc, 0x57fd4977b16ee, 0x63e030388d50a },
        },
        {
            .X = { 0x61904a63f000f, 0x1bd716fe62caa, 0x14c5a5e251f84, 0x3dfd95553c0fd, 0x480d816072eb6 },
            .Y = { 0x684a8cd33137e, 0x5886d75ece3ca, 0x6654b005abc5e, 0x1dabf39c3ada4, 0x552d0f1d6bda4 },
        },
        {
            .X = { 0x33f7e95221da8, 0x4e0a7caf97d67, 0x7d63278c8fd1d, 0x61125b15491dc, 0x148a7d88b200d },
            .Y = { 0x2dfead4e889b4, 0x6aa628cb3c563, 0x20f683e3e00ab, 0x201e3231a2df0, 0x4f4f0b1a20843 },
        },
        {
            .X = { 0x33f84ff1f58e9, 0x4ad261c396392, 0x3e17c51f865d6, 0x54ebc546e84fe, 0x130130d18b59c },
            .Y = { 0x1894bc50376ff, 0x2b38e0b200e13, 0x7f9196a244767, 0x3a295aaecee77, 0x29c224fb2db0a },
        },
        {
            .X = { 0x38d46484879b, 0x2976e11fab93f, 0x7d53cbbd946f5, 0x1bfe5c9fdf184, 0x36c2e3fa23db3 },
            .Y = { 0x13ce53eeb864d, 0x4e4f0eeaf8fbc, 0x3bc3ac9f0bc22, 0x69ac3e8c9a214, 0xc5c488542844 },
        },
        {
            .X = { 0x482ac0bf80638, 0x51efc120572f8, 0x2419a0021b2ae, 0x278fc16937ba3, 0x378e98824a6bc },
            .Y = { 0x23fd4b8eeb48e, 0x68ea7a15c0376, 0x1ee4b123a88a0, 0x698dc36b7307f, 0x114214780d621 },
        },
    },
    {
        {
            .X = { 0x2817b94e68325, 0x47920a1d5d236, 0x64962364b5a2, 0x8acb90b0b095, 0xea2c846a535a },
            .Y = { 0x71b5a8b0d031b, 0x3e7a622de4972, 0x6f860e94280cc, 0x8ddd60bb5089, 0x3b82a8337b05f },
        },
        {
            .X = { 0x77dbcd25c5f3d, 0x88a44da541ce, 0x71a9b1b77101a, 0x748c3b6d6a4eb, 0x561d2c1d673a0 },
            .Y = { 0x5d2d4cf747afe, 0x46e1b7a1bc33c, 0x733b3b1b799a4, 0x135ee3bb51984, 0x2dac09925d762 },
        },
        {
            .X = { 0x33c6e4ad2af6, 0x73dfbca7e8058, 0xa61f4acb4073, 0x63dc8371bd9d, 0x70231a092565f },
            .Y = { 0x10642c985a6c2, 0x4aa22eddc348c, 0x7d48d59716dab, 0x5cb2953e0a5d6, 0x1df8dc634575b },
        },
        {
            .X = { 0x2dbd3eb171523, 0x595723e002bc2, 0x52c6fb6b33904, 0x344b30bedde16, 0x140b6f1e4bad4 },
            .Y = { 0x2d77b6bfce11, 0x14bb469e30449, 0x35d50f849c2c1, 0x7a9ee1395876c, 0x444eb3598ccaf },
        },
        {
            .X = { 0x652250ab38365, 0x3a020ea8eaa8a, 0x458f7fd3d3e51, 0x3dc086535d721, 0x34bd49edfa2dd },
            .Y = { 0x2217edfd34c25, 0x340c1c16cf1a3, 0x1b5323bacf59d, 0x72f1bc4be26f7, 0x73feaa96bbe61 },
        },
        {
            .X = { 0x28e8a83744eb5, 0x442affcde176b, 0x2968dd2043343, 0x6b0d71cc909b2, 0x70d99008ffcb5 },
            .Y = { 0x6c54e726c32b9, 0x44283befd1791, 0x67e09c40e60ed, 0x2c997194d7c90, 0x56b125243a069 },
        },
        {
            .X = { 0x3abc449a3d1a4, 0xa2263753947e, 0x7dfc5b0b50798, 0x4dfc6e993065, 0x7b50476b0a337 },
            .Y = { 0x12a1e8baa33cc, 0x243b17bebcaf6, 0x3c6188ed37727, 0x6a1284ec80951, 0x2448c3ec7026f },
        },
        {
            .X = { 0x1b2660f617607, 0x508bd980fc724, 0x4d510c8a68608, 0x665aa2f30af09, 0xa9fcf41f6ef7 },
            .Y = { 0x2e6c384daf9dd, 0x72bb2da4493f1, 0x64d5b063161bc, 0x1d4691dbcdad5, 0x732fb4faed167 },
        },
    },
    {
        {
            .X = { 0x2d644c7dad28d, 0x43703afa4db6f, 0x1f85df5ea777b, 0x73e16c6821b8e, 0x1bc7af1e38185 },
            .Y = { 0x2f65900314833, 0x24c6364e1f95e, 0x57701247409f8, 0x797bd2f77c3bc, 0x61d909d855661 },
        },
        {
            .X = { 0x1938218028354, 0x6bb1b54fa00f3, 0x6e28f67cbea25, 0x4b5e9141aad35, 0x6bce245f8c25 },
            .Y = { 0x4955188a3c065, 0x161bd0f1292df, 0x1d521630b2506, 0x6d06495669788, 0x3d26989cdd0f6 },
        },
        {
            .X = { 0x589041b29662f, 0x1d6fbadddcfd7, 0x1fff2d032c3bc, 0x2f7b21e1d64e1, 0x7665908aaf444 },
            .Y = { 0x3a00e4b704ca1, 0x71715f4826e10, 0x40ad025948864, 0x2a3670bfc0327, 0x4ed18a7c50da4 },
        },
        {
            .X = { 0x5954e6c16d4f7, 0x7ca457221ebb3, 0x69398c394d173, 0x554841a79f99d, 0xa09b36eb5a04 },
            .Y = { 0x2eb0fa35ed926, 0x49a86e7641e74, 0x3b54cf41b3a7b, 0x85125b7595d7, 0x3aa47fd60fa31 },
        },
        {
            .X = { 0x4d5451113735e, 0x2574dcfa41233, 0x55926182e858b, 0x6b540645ad45f, 0x1c71ce37638e6 },
            .Y = { 0xfee129e406cb, 0x7e88f52e59c7d, 0x2833edf503460, 0x166921edd6bcd, 0x7880cc1c2003b },
        },
        {
            .X = { 0x39e7f74cbc8d6, 0x8d90a5963263, 0x70b3f944839e5, 0x7bec4bd417c4a, 0x3c607a84c1df2 },
            .Y = { 0x79a86ba3cea68, 0x42e6340c19d4e, 0x76fb861261f82, 0x51d11c25d5a44, 0x34518ad109941 },
        },
        {
            .X = { 0x2f417ffbe2da0, 0x4d4c71c738621, 0x5bbdfc4e85ff1, 0x6331ffc9c6eb0, 0x89b6cd02000b },
            .Y = { 0x16f13e8ceb88, 0x4f9b762547cf1, 0x72995f90f2f92, 0x7ec5a1c6f7e88, 0x66ef7206aa36d },
        },
        {
            .X = { 0x6b2d6fc2c9fdb, 0x5b0e0cf4a45c3, 0x1de41b4adca18, 0x6b07c3d5aa1a3, 0x79e9d5b60917 },
            .Y = { 0x1381c04c78797, 0x79470b8eb0720, 0x2fe8895900193, 0x5229893654a28, 0x16512951d2240 },
        },
    },
    {
        {
            .X = { 0x319170c5cbeb, 0x45dfa1901b262, 0x4f5a78e09bfa1, 0x61a937098bfc8, 0x5e9ac4ab2f4a3 },
            .Y = { 0x6c32be5b51bab, 0x1fcce7a160521, 0x3879392f3c991, 0x1e65e9799a94d, 0x505f00fa8aee0 },
        },
        {
            .X = { 0x4b53d220ddbf6, 0x73c503e062ea2, 0x6684612a688de, 0x54bc44a9da83, 0x2e5c4ef1a1273 },
            .Y = { 0x3dc4dfb54008b, 0x196ffe806a2f9, 0x13a653b912bfa, 0x1cb2e2eaa3a54, 0x5c1731365512a },
        },
        {
            .X = { 0x41f5395c68f4b, 0x21527553fa46d, 0x449133ccc4dae, 0x20ad573c8d918, 0xe2606a00097a },
            .Y = { 0x150dbc87749c3, 0x10772f531215a, 0x4ed926dacae76, 0x78e59c7d6a1e8, 0x2dda446c4a628 },
        },
        {
            .X = { 0x7f2867bef24ce, 0x710efab9d0ee1, 0x1ca33cabfbcb4, 0xb725aadb7597, 0x4c6c9a0e8a4ef },
            .Y = { 0x74fd1c286ea25, 0x2cd1ab9507d6, 0x2e4153317b496, 0x44139df386b6c, 0x26d5711ee5a1e },
        },
        {
            .X = { 0x55142c59d25f5, 0x1105332feb39d, 0x5a3b10bc1a673, 0x2fef4bf5b0200, 0x6a73b1e2a603e },
            .Y = { 0x43f6ebcfaffcd, 0x7d7801e33722b, 0x41d7ad7f6053a, 0x28882b8c39aa5, 0x6ea5ccc1efdb4 },
        },
        {
            .X = { 0x5224fa7c36d4e, 0x6fee2cfd444c8, 0x1c4a8061b718f, 0x798676199ae1c, 0x728a71912cf9f },
            .Y = { 0x123d52809448c, 0x358c8b09e71f9, 0x64d5f1f4bfd7a, 0x507b16eb4e0d1, 0x4ce95d59305cd },
        },
        {
            .X = { 0x544cb218af30c, 0x348d6ce5b8f28, 0x316d36d34d1d9, 0x1985a3b2895e8, 0x5ca076f131f17 },
            .Y = { 0x44971a4ebf223, 0x717a30b3a1a67, 0x533a8b44436d8, 0x441bc8bdcf71, 0x314d831f8052d },
        },
        {
            .X = { 0x4d77227507e92, 0x86b032cd3ac1, 0x18aa9b22cd652, 0x1b6798a8e3743, 0x599ba21b65ffb },
            .Y = { 0x250c48865a9e2, 0x288bedc7777f, 0x737db77525f28, 0x615b3cbf29f9f, 0x7f59b350f668 },
        },
    },
    {
        {
            .X = { 0x723d42c9bfadb, 0x4c0ed8c71458c, 0x14f480f093d20, 0x4a3b8b2f69410, 0x3328bc121616a },
            .Y = { 0x333cdfafac039, 0x232736ed2f8e8, 0x5319c6317adca, 0x65939f2fa7808, 0x2bbf3f26c21b5 },
        },
        {
            .X = { 0x76fc37c90a733, 0x542ee4cf414a2, 0x7077a0c518dfb, 0xf9bd192096b8, 0x3646a62ad82bd },
            .Y = { 0x3b8f97c6bccac, 0x671efba0b8111, 0x4032a1b6883a9, 0x4f474a6b9d6e3, 0x24dd06031fdaa },
        },
        {
            .X = { 0x3143712372646, 0x5066f24482e5e, 0x715c90f9b9e3e, 0x5bb5858157a21, 0x65a10ff6a067c },
            .Y = { 0x669777c684d1b, 0x213883cbaabd4, 0x5fbb31ae52c4d, 0x65f0e5e478f14, 0x3607014f6a64e },
        },
        {
            .X = { 0x52a980a61b29c, 0x16cb5953ddaf4, 0x61772fb88782b, 0x29a2bd34e07ac, 0x3c0c1e7beb87a },
            .Y = { 0xb50d6384bd2f, 0x698d273c97e14, 0xbc65cffcd268, 0x318c91622aa37, 0x74e826b2f1347 },
        },
        {
            .X = { 0x748fe1a825c77, 0x21514a7862c84, 0x34c40dc979953, 0x52999cbd4a7a, 0x579ac4b521848 },
            .Y = { 0x2b413d28a281d, 0x60c9f45a12b5c, 0x3f1dd08f82872, 0x57e772b88cc31, 0x62bfb93d6971c },
        },
        {
            .X = { 0x19eaf801c2154, 0x1be0322c74e42, 0x53201cb48f8a9, 0x42e4f4ad023a1, 0x61eee376f0dc8 },
            .Y = { 0x17efbf8b38931, 0x3251e4b970675, 0x517806e2abde0, 0x629fa3a1464e3, 0x1f2abf8d3ed6e },
        },
        {
            .X = { 0x7114d8a25906c, 0x4caf6d59c324a, 0x390d2cfa230c3, 0x601e8430d6319, 0x3d12191c75243 },
            .Y = { 0x44e5e9acdfe83, 0x30c658bfb78b8, 0x7ad78f301fee4, 0x3c5e4cda71e2b, 0x467965880e34c },
        },
        {
            .X = { 0x44c0c6d90670a, 0x2abd4f7bcd819, 0x688466d633631, 0x29e0221d8a5dd, 0x53b1cdfe23ed5 },
            .Y = { 0xaf01f575fb05, 0x685209f8b0e47, 0x1b94022899de8, 0x725c6ab995e8, 0x5ca6df45ad963 },
        },
    },
    {
        {
            .X = { 0x49581c869b64c, 0x879d26924672, 0x8a70dd68f436, 0x6e15a6732d550, 0x1074ff0267662 },
            .Y = { 0x1efc7e6d5b022, 0x168c1b4274f6, 0x25dc041f50b06, 0x7614491ed2682, 0x299be0decb680 },
        },
        {
            .X = { 0x62b362a42fe33, 0x39916b8c86c5, 0x7a09e842055aa, 0x4ac79473c72ab, 0x3e63c4ae42121 },
            .Y = { 0x680caa79c9678, 0x3808f9630a055, 0x7b6eb04996f05, 0x4464642b82cd2, 0x3599286aac87e },
        },
        {
            .X = { 0x287be2808049f, 0x1b3fbc705101b, 0x518bbed8b8f93, 0x4ea28835980c6, 0x1eade2fa113b1 },
            .Y = { 0x3ae0a699920a0, 0xb6f00c89ce14, 0x1550d0d521e72, 0x53254be46b6c0, 0x6cc8d8212e6cc },
        },
        {
            .X = { 0x17508ad074a91, 0xcdf8765489f8, 0x59ead4b3ac4f9, 0x6ebb635a42a04, 0x31ddffcb76711 },
            .Y = { 0x736f0fdfa8836, 0x2ba0a110e80ed, 0x5901cb92128e2, 0x6938d00bb08cb, 0x221c9b9a258e4 },
        },
        {
            .X = { 0x5d57ccf82cd76, 0x191ac38152a73, 0x527db82fb2d30, 0x298d4ffa67db4, 0x4af6009911d4f },
            .Y = { 0x1a6e40365167c, 0x2746419610dfd, 0x153e95df1a57a, 0x22bf1f6cf605, 0x1ea69bf6f0ee1 },
        },
        {
            .X = { 0x7a92bd11bba55, 0x6c23a2371bcd6, 0x35bcff65a49e6, 0x2fbfd0c597d5f, 0x1ed4bfcc333a7 },
            .Y = { 0x3506e47909a5f, 0x4d66972f03198, 0x286327e683fc, 0x1d118b8849c47, 0x73d969f556be7 },
        },
        {
            .X = { 0x3683fcba71031, 0x202c546ccb866, 0x7e7b4570962d, 0x715e4d1647212, 0x3a4ba693d760a },
            .Y = { 0x7961ef3ecbb5d, 0x241543ff2f21, 0x8a085d8ae874, 0x17e8ba6de5d32, 0x438b99022fc52 },
        },
        {
            .X = { 0x6ac0f0ebd57be, 0x20575ae346ecb, 0x5775567b00914, 0x659a612e62254, 0x6b265507695b3 },
            .Y = { 0x32951a7d46b30, 0x5f0454eaf5f3c, 0x75600ae647758, 0x5fee54e8a8f33, 0x3f7f093597ace },
        },
    },
    {
        {
            .X = { 0x3c41cb05dc3f, 0x4732705808479, 0x71ccb4fe555a3, 0x66f2e8dea7d5f, 0x75d942c04210d },
            .Y = { 0x5c4874e35ab2d, 0x5a8f4848cf2ec, 0x45cc72fce38fe, 0x9e47ebf162bb, 0x696cc14856cdc },
        },
        {
            .X = { 0xa57b885fae14, 0x48bd176421bf2, 0x6483f528f0c5c, 0x6bb56ad0c8d30, 0x6d12e0dd1cd8b },
            .Y = { 0x4a3028f70bd8e, 0x6fb588ceb49a4, 0x15e96e043c83, 0x3c01505c1b392, 0xd5c9e4b74fde },
        },
        {
            .X = { 0x39f2d199b0017, 0x689cd6da39047, 0x4ee67cfea426, 0x44f62eac8c288, 0x7d04264e36e88 },
            .Y = { 0x3319ae930f866, 0x2b7f27fb1bc34, 0x68ccfeaf09e3c, 0x13fce3bbcbc54, 0x6cc9f269424b8 },
        },
        {
            .X = { 0x3fba21f5fd594, 0x5a41f0680fc3d, 0x60684b418c206, 0xd78588e79b4a, 0x16c1556c9a2bc },
            .Y = { 0x335745ef14f36, 0x3d719e23ae502, 0x5ced6e016ce4f, 0x7c48f0cd6f5f4, 0x23e38cd5158b0 },
        },
        {
            .X = { 0x11af2b10518f5, 0x1b54232605b64, 0x63b6355b3ae8c, 0x634abcb5fe74, 0x1723bcb3e1d15 },
            .Y = { 0x689807682b3ea, 0x769004a1eaadc, 0x1faf5e2abddd7, 0x591a01cc06d86, 0x5d4ba682e08eb },
        },
        {
            .X = { 0x70fe618e77933, 0x9554f402b26a, 0x57a55d0c13eff, 0x5807222ada534, 0x456bdf4c7423e },
            .Y = { 0x50b42c46c2997, 0x2e6936b85381b, 0x1d18e01e71f42, 0x43e15d81bc772, 0x579782c36c68e },
        },
        {
            .X = { 0x6a339f4412193, 0x50756eb9eb0a5, 0x1767916351622, 0x1089da2b5dd8b, 0x2269a46cdf0aa },
            .Y = { 0x68f85514c514e, 0x7083ffbf804f6, 0xb87e873a8780, 0x3f90dc508ecec, 0x489cf3771fa99 },
        },
        {
            .X = { 0x3804bb683be22, 0x3733debc4886d, 0xcee3497f2a2, 0x5bb4af39f15ca, 0x66e632db15cc9 },
            .Y = { 0x2d6a6134af084, 0x38fdec0e8d27f, 0x1239e9bd979b5, 0x660c87ff50378, 0x534e3c9c194cc },
        },
    },
    {
        {
            .X = { 0x309b7ada7ee07, 0x43cb957e9c921, 0x3cd9f56bf9b98, 0x2d3dc9be3f67, 0x7511066c0e121 },
            .Y = { 0x6a33ba386fcd5, 0x626dee17f6147, 0x29c02ddc3696e, 0x65e544247c46d, 0x604df365b8391 },
        },
        {
            .X = { 0x1d26ae8619807, 0xe30b7aa4a930, 0x2b51a914548d7, 0x5c88c2fe4868c, 0x668d5960c0b43 },
            .Y = { 0x6246e53791b03, 0x110a12b1b2af5, 0x45378ad74e89, 0x58f49e437341, 0x7a66f757c2190 },
        },
        {
            .X = { 0x2d8352e175264, 0x601e11692d66c, 0x4aa7039c9423, 0x107f06a461a90, 0x158af2b748828 },
            .Y = { 0x2530bd1561746, 0x519a6f2a4a98e, 0x4ebbb5934b808, 0x40ebebae7fd41, 0x627e7cd734b03 },
        },
        {
            .X = { 0x522d70389e30f, 0x5b3d1b69953f2, 0x41d648637fe5d, 0x3227c2f1681f7, 0x5089c58085e74 },
            .Y = { 0x186248653d88b, 0x1d8641f402a45, 0x250d1eeb72e55, 0x20cb7d3d5b98, 0x50cab959cb2b6 },
        },
        {
            .X = { 0x140abde7555f6, 0x4ad61ed9ff36b, 0x7923661b7033a, 0x1ca1d9b3e7636, 0x7d5f6473e7c2f },
            .Y = { 0x1179993df2540, 0x2aef6937c200c, 0x5418fd61513e1, 0x17312d93ebeac, 0x3c49c73bc2ba8 },
        },
        {
            .X = { 0x24e97e3f868a1, 0x621f1173b4625, 0x507b0c7fb94ce, 0x29e4c0eedfc23, 0x5dde3f530354c },
            .Y = { 0x484800427a7c8, 0x26e9045f15da5, 0x33841231904d2, 0x79b14e162e6f5, 0x759527498e0c4 },
        },
        {
            .X = { 0x7f4fbb4aa85b4, 0xc579767ffb50, 0x8213dfcd1205, 0x68281deb8486d, 0x3d5a995a523d9 },
            .Y = { 0x4b566edf58220, 0x55894c276321d, 0x7ad74afff357d, 0xd3a9a539d392, 0x706663d330b1d },
        },
        {
            .X = { 0x21b23789b5bc2, 0x313614bf29107, 0x3ffd8454db8ae, 0x400166213ee3, 0x761affb1e0d4 },
            .Y = { 0x6a170812fe6b0, 0xe5988f808725, 0x6b25da56d2916, 0x29dbe21081636, 0x3a6d110e60701 },
        },
    },
    {
        {
            .X = { 0x3d525e99d4787, 0xb4f0bbef08fc, 0x469e57d31e97a, 0x6ce5014595179, 0x28bd38d44030d },
            .Y = { 0x41d6d78d5bbfc, 0x748889556933a, 0x2e666d3fb606d, 0x1b99d69e273e6, 0x552dd7405c418 },
        },
        {
            .X = { 0x7dce55f7be126, 0x44884d4f42fa7, 0x6d7c5f63f008c, 0x7baea0cbda388, 0xec8fe27a51e6 },
            .Y = { 0x52cefab3d11d3, 0x570264f87b63d, 0x701ecf073e3f9, 0x53fbb3adc6fb0, 0x3de83daf93d8a },
        },
        {
            .X = { 0x27eb68d4e5bfd, 0x22d2099ddf370, 0x6d54a83bffd49, 0x66b6f17af0bbc, 0x190bb443a02d3 },
            .Y = { 0x6f417484861c0, 0x645fd45aa2313, 0x22edc528f83cb, 0xaee0c0fa981d, 0x5a601f5562397 },
        },
        {
            .X = { 0x5db6c768a89e5, 0x5cb33ae4b6149, 0x3eb8df0618d8d, 0x3d2fa46d0edf1, 0x393fb3d0b216d },
            .Y = { 0x776f56c0960ce, 0x298f2c7500482, 0x16d7bdafdcba, 0x2ed28bde9f7cb, 0x4e2107bc67440 },
        },
        {
            .X = { 0x69d00957c9907, 0x2660e070fd1e1, 0x6793524f516a2, 0x5777daf72d122, 0x30d3831392a57 },
            .Y = { 0x5cc64cff559b9, 0x64453c75f705d, 0x5800bb22610fe, 0x40e73ee010047, 0x50e644a29bc85 },
        },
        {
            .X = { 0x73d7b00d6838c, 0x372af3f3d7ba3, 0x5fe45b35dbf2e, 0x2f823d692b6e4, 0x18b612324ccfb },
            .Y = { 0x3d75788f5a8bf, 0x6c1a1a3b440aa, 0x2cf4d85d1d3c5, 0x63ee89401b498, 0x4b6dafa36309c },
        },
        {
            .X = { 0x7b2eca0b600c4, 0x7cc0e78963ab0, 0x184246ccec92f, 0x3b54e750110bb, 0x6e847af3ae326 },
            .Y = { 0x199f7a2182148, 0x1bd722e8edc1b, 0x6eed3a0e99d47, 0x34de4aad941b2, 0x362ea253baea2 },
        },
        {
            .X = { 0x2deb9216f73e9, 0x4620632fd6fa4, 0x63facdf1b87aa, 0x7073220ef2c21, 0x1edff13d99073 },
            .Y = { 0x20617c243adf8, 0x2e319b0d53c9c, 0x274999d321e3, 0x131536578d28c, 0x2e42eb00cbc19 },
        },
    },
    {
        {
            .X = { 0x1132c34ff961f, 0x677d0b1215964, 0x2be5f787a79c2, 0x6a044a47b984b, 0x5830f025f1078 },
            .Y = { 0x717e25793d41e, 0x406aa79573bc, 0xc0795b6cacbe, 0x1fff337239217, 0x67d451752c243 },
        },
        {
            .X = { 0x4e15da1d9062b, 0x538d34783da3e, 0x36301e04c4cac, 0x4e4d4cf168b8b, 0x19929124ab577 },
            .Y = { 0x175a69ca1fb69, 0x88480e6c0fa9, 0x41079e654284f, 0x4060f4dccca41, 0x52c0cbef541fe },
        },
        {
            .X = { 0x3720f2c76158f, 0x54598b784e873, 0x1658af745782b, 0x5929eccd5cb86, 0x633198a5af64e },
            .Y = { 0x40de64a7b0c18, 0x7a060f97789b8, 0x30a09a3c7b09c, 0x73b47904223a0, 0x888e9e9da049 },
        },
        {
            .X = { 0x28737a1738914, 0x345db23b9ef4d, 0x673c301d3b59c, 0xbddf846fee99, 0x849c9e6da66d },
            .Y = { 0x60ab0555edde9, 0x1a85cb49843b, 0x313c5ea495cea, 0x1c96497d7a016, 0x9f4dcbc5ae50 },
        },
        {
            .X = { 0x5cd31fd1e41bf, 0x59b5fd0d0b914, 0x340e41ce5965b, 0x66ef688d92dc4, 0x1deb18e4d05b2 },
            .Y = { 0x20912f604fd5f, 0x42b3fcd320750, 0x2a01c840d23c, 0x220cb51fa5db5, 0x4ac1d5c8cb78e },
        },
        {
            .X = { 0x86057c2be1f3, 0x7d1f695a95270, 0x747bf37f43265, 0x220899a34967f, 0x1e0990d03bcf6 },
            .Y = { 0x245e680d6459d, 0xe68b9542be95, 0x58a109a328a66, 0x2f5296014db3e, 0xea9350f1881f },
        },
        {
            .X = { 0x3d3fb9791b943, 0x3d4aae98c1e45, 0x1b7a57195d8e8, 0x4622b7eac6924, 0x1cb7a40c1d8d4 },
            .Y = { 0x6dbaec67ef776, 0x2493599c90707, 0x733b30c4de9a5, 0x8e9020d152b0, 0x2835019689be9 },
        },
        {
            .X = { 0x7946d7c7220e7, 0x65e3c6a888a4b, 0x24ab83d18dac2, 0x6d8bcb74eb874, 0x3dec05fa937a },
            .Y = { 0x4271cc6375515, 0x949949e28da3, 0x56bf0e5189623, 0x17ab48894ff72, 0x15949f784d847 },
        },
    }
};
