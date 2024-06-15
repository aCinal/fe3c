#if !FE3C_ED448_SMALL_PRECOMPUTATION
    #error "Build system inconsistency detected! precomputation_ed448_small_64.c in use despite FE3C_ED448_SMALL_PRECOMPUTATION not being set"
#endif /* !FE3C_ED448_SMALL_PRECOMPUTATION */

#if !FE3C_64BIT
    #error "Build system inconsistency detected! precomputation_ed448_small_64.c in use despite FE3C_64BIT not being set"
#endif /* !FE3C_64BIT */

#include <points/comb/comb_ed448.h>

const ed448_precomp_internal ed448_comb_precomp[57][8] = {
    {
        {
            .X = { 0x0, 0x0, 0x0, 0x80000000000000, 0xfffffffffffffe, 0xffffffffffffff, 0xffffffffffffff, 0x7fffffffffffff },
            .Y = { 0x6079b4dfdd4a64, 0xc1e3ab470a1c8, 0x44d73f48e5199b, 0x50452714141818, 0x4c74c393d5242c, 0x24080526437050, 0xd48d06c13078ca, 0x8508de14f04286 },
        },
        {
            .X = { 0x34e32ee005128a, 0xc4a9487e0e9971, 0xfadc4e769de751, 0xca611cb550d150, 0xf8ea62e2bce16e, 0xd9bd95ccc736a2, 0xc337f93b324d1, 0x74e9dd52d1810b },
            .Y = { 0x7cfbaef7437683, 0x995ad15df28d32, 0x5cec07ce02191d, 0x15f74e73f1e831, 0xc5a69d045ef70f, 0xbe15209d7069c9, 0x97f22934353384, 0x5e450dc284cf43 },
        },
        {
            .X = { 0x3d9a138ca5ec33, 0x5f7f210df60a21, 0x57d58d06961ce8, 0x96ea76d7febe3f, 0xb2c76b1746ac90, 0x288d0d88dfe254, 0x21a6150bea5a0, 0x1868374623eac9 },
            .Y = { 0xdadc0ca702fd9e, 0x63ab46bb343ae6, 0x415a946e63072d, 0x4b1d2e18b3cd1a, 0xcc8367c24689da, 0x291a9cb4596e1e, 0xaa2d353a6c67d3, 0xbd6764f3867273 },
        },
        {
            .X = { 0xdb91fe05eda2aa, 0x6c633236921c7d, 0xbb2a9a393676f3, 0x4c85c5c01b57cf, 0x3f46353e51950, 0x2311f3d816d0cf, 0x2e3a72bca6d46e, 0xb3c55fc2cdfe0c },
            .Y = { 0x6aaecf1f07a5c1, 0x9604b2f63aed2, 0x694fbb8f34580a, 0x2a3e4259e08feb, 0xf36c8db92cd293, 0x24195eee462a08, 0xc9dec279c8e98c, 0xb38349f993aa40 },
        },
        {
            .X = { 0x320a7612a538ee, 0xd94f60a04dec61, 0xc5a9eb0eaaa73b, 0xcfa0ae962c2860, 0xf6587e64bc401e, 0xe46e1840a5f163, 0x4e6fee85ee03d0, 0x3395a9bbc97d68 },
            .Y = { 0x4be5ffd0242cf7, 0x3943eaa056cdb9, 0x22e917feead966, 0xff54d2a7b62545, 0xf10397f468edb5, 0x53be66919fd676, 0x76b8d42c763403, 0x6bd98c8968990a },
        },
        {
            .X = { 0x7afa3c98ae4fe5, 0x9410e2c16341e7, 0x5bf4f95ec13419, 0x17aac74894f4ca, 0xa78513c323270e, 0x968f1ba4a37eac, 0xda0c63c24ab0d9, 0x90e9b614b77bec },
            .Y = { 0x79f31c7b76b25d, 0xb998bb56c85187, 0x438acdf69b8612, 0x8c8066e64b4ddc, 0x5c11ddd9715a3f, 0xecb117d922dd72, 0x3afeb742d37dad, 0x51c527438043f8 },
        },
        {
            .X = { 0x54920cbce3d732, 0x21b6dfe17cfcd5, 0x16eb249407c709, 0x368496617b1e3, 0xe8281d4e40c760, 0x8cdb075188ae50, 0xc2fbb5eb5a688a, 0x4cc5a79cb3f005 },
            .Y = { 0x2a2d2e40a517c5, 0x34a60697a63c53, 0x20825bd640eec2, 0x42e2ffcef8ba7d, 0x7fab3686390d00, 0x3baa3c83b92237, 0xa12724cac27ae7, 0x360fd610f89089 },
        },
        {
            .X = { 0x8b89fa0b8e53a4, 0xf647d8d58ea17f, 0x9538c0899d04b6, 0xe8fa1c12895fc2, 0x450d6ccf0930be, 0x2abc43a6af24e8, 0x4bb9389a327c73, 0x424ce7de4ebb3b },
            .Y = { 0xdd150c0e4e6e5d, 0x22347886819e6e, 0x7bc067231bf14b, 0x64613e03307c04, 0xa5c6c958fd8d58, 0xd2f3e9f48c5966, 0x4f620bc214e93f, 0x1413c564c0ca24 },
        },
    },
    {
        {
            .X = { 0x9b9eeb3ccb549a, 0x35a70990a89d68, 0x8777f551ec2cf2, 0x7177a1903655ea, 0x97db5ed39dfdf0, 0xa481adb8baf4a6, 0x2da3920de8379, 0xe6a0d7ffedcd4a },
            .Y = { 0xf8cc2d655b519e, 0xd3c605f2c8e5d, 0x91fe82739462d3, 0xeb8087096df06a, 0x176a60ab92b93e, 0x50d5f8722b24b1, 0x1ea3e7df1ff0b8, 0x8b528f5b195fa4 },
        },
        {
            .X = { 0x99b406d04cd811, 0xfc95b8006f769f, 0x813e6647166f52, 0xefa42fad77b8ca, 0x4ef30ba698d4c4, 0x3aaa6d9a613e9d, 0xc09a45deb591a1, 0x907d91338d86a5 },
            .Y = { 0x7b73307a284211, 0x1bcad3ee468f48, 0x19a5033b35866d, 0x6fe02b1ce427ac, 0x2fcea30d1af7f7, 0x7f0b3a1f845140, 0xcfca1e7150b086, 0x85bd74f32c4a09 },
        },
        {
            .X = { 0xc76bae07b45b6a, 0x3be0c3359da38, 0xae239ecb8b22d9, 0x625fb35fe44a0e, 0xdcab3e50d62792, 0x1532441ee28f44, 0x5ab1cc2bf13030, 0x42d33a155ad69f },
            .Y = { 0x1f0d0c4149cec5, 0x227e4e7bf8580, 0xae52ade74a02e1, 0x4beaf49159829f, 0x27dc9afd9da44b, 0x8eda4218a184c6, 0x226ea21039c3d8, 0x863e81bd6cf61e },
        },
        {
            .X = { 0x36a6e959ccf864, 0xa9137f67537624, 0x639b7b3fdb9a74, 0x7284774d7e4251, 0x8d4992409487, 0xe7174840d3a24f, 0x92063ba243e46e, 0x8f8c6690788587 },
            .Y = { 0x3b68fab7103560, 0x401308289d88a1, 0x82839089d6325b, 0xa43771fea99363, 0x1a7591c912e52d, 0x2c9e3be428108, 0x225c81c24d2228, 0x524fd818c43b25 },
        },
        {
            .X = { 0xaea9fec33b1d06, 0x8b1916143b0a4d, 0x518f28f79fba78, 0x3d050825028cf7, 0xaf7006f559e825, 0x68b2aeb924f395, 0x9ce926936fbba0, 0xa46b8c58d16c3f },
            .Y = { 0xa765db7c64821c, 0xd04ac43e682eea, 0x9defde8d8ed06e, 0x49594149915250, 0xf2d29fc84c5756, 0x84ea7708c1b448, 0x100eb37569a96d, 0xeeb7fde9c4ef30 },
        },
        {
            .X = { 0xb0f01012878eaf, 0x7239464a2f2da3, 0xdd629ab0efcf15, 0xf29e64c9ca0e98, 0x4a1c8dd7068a8, 0xeea0a5475ba1a, 0xe5bea4b4d8a341, 0xca0f34e831a3f1 },
            .Y = { 0x248fd01271b48b, 0x1689653f5f42f9, 0x4698516d216325, 0x5d7884f1ddf3cb, 0xc167e1a5004ca1, 0x1f9ceca4c0bb7, 0x9932494f89ae3d, 0x206756bf0eb1a1 },
        },
        {
            .X = { 0x498087be465a00, 0x96526fa40dcf4, 0x2b80f549f3a092, 0xdff18e83377efa, 0xd76b2c9cffcd6f, 0x46b286a781b34f, 0x465e9a50da1685, 0x133af4da552605 },
            .Y = { 0x2ad746511ad1b8, 0xcb4dda0f0f3b04, 0x72edc6b2015ceb, 0x566f93a54d52be, 0xe7f84c1bfaf0e, 0x3fe1302ffe7e3c, 0xbe87c546c4977, 0x79a36fd90cca04 },
        },
        {
            .X = { 0xaa622b55fef44c, 0x36b82bf7161b10, 0x7c67e683039d0b, 0xf9ca0a78ecd0bb, 0x9cb1420becfc50, 0x13cedbaa4afba3, 0x1d39a6ca2f2b87, 0xff146c8ec9e8dc },
            .Y = { 0xbea38902b63ded, 0x998585277b2ca1, 0x34fda2c1bf6fcb, 0x1bf2044b78efd7, 0x84ff5ac83a9984, 0xb3ec5aaaeced00, 0x11df507cd3b347, 0x8a5b629207a240 },
        },
    },
    {
        {
            .X = { 0x14103d83a5549, 0x1026439dd613fc, 0xb96e899967bb63, 0xa9556b527bb47e, 0xc0be6b0ad61c16, 0x65930d3278ffde, 0x4afe9330794cdb, 0xf86411345ba38f },
            .Y = { 0x88c02d9c9a78a4, 0x38eeb660be3168, 0x3a105b70c15c8d, 0x65b67d2019aa45, 0xd57dab68e082e0, 0x28da6b2ccab2e7, 0xbe22da6d3009f6, 0xd6e2a39841c095 },
        },
        {
            .X = { 0x2e66ce7b7cc7d7, 0xf422a152a2e522, 0x7b70eeba6f667f, 0xbb7c19d11adea1, 0xc9084199df04ca, 0xed9bc8096f13ee, 0x9de546f6a1abf9, 0x1d9137ab90d27e },
            .Y = { 0xa0b48871c765c, 0xee32fba8167e8c, 0xdda38a5d0d4301, 0xfbad15469ef41a, 0xc43573f2609824, 0x184a47bb9e7bf3, 0x6721e6d4481a3e, 0xfc7c0bbc960077 },
        },
        {
            .X = { 0x76670a77ae646, 0x5a7fe5254ac412, 0x265f95ae80c896, 0xa77f2d8ad18d4a, 0x8490653e1eefc5, 0x854bf05cc4d9f2, 0x286b31d864df9e, 0x54f4878cb80707 },
            .Y = { 0x91ed0d5413e2f8, 0x67263a7f5ae674, 0xf4994a1a428878, 0xeabe7f9636b6c6, 0xfd0d887896bc9b, 0x361ba45c824d18, 0xd5c7badb0a6dd4, 0x99c0ec3e39a7e },
        },
        {
            .X = { 0xacb7d556c92e47, 0x1716902796699f, 0x29ccd248109162, 0x2153bd34f89888, 0x574209cbeac938, 0xde2c0663bfd7a5, 0xe205a246a6d575, 0xb2a0a666944fa },
            .Y = { 0xf11854eefc0c77, 0x39a4509f901e45, 0xcd2ff34636063e, 0x3dca45ab113f73, 0xee29ca46302956, 0x147ab7f6fe92f4, 0x61b97c14f22a5a, 0xbba1c0caf48fd4 },
        },
        {
            .X = { 0x4de851ee75ee3d, 0xb1dc37ca1661fe, 0xfa20c4d06a199d, 0x36b32f023c4d9a, 0x56dd124959f513, 0x1f8caac035db7c, 0x73522f06663d1e, 0x976e9e620536c8 },
            .Y = { 0x1045d1887e5232, 0x6a4bd1d89c7b15, 0x2e6628e42e7dfc, 0x3f47be6665247b, 0x1be3572e3a9647, 0xcac6dca5250a9d, 0x32b6117770af04, 0x134c35e1f2b8b6 },
        },
        {
            .X = { 0xca365829ee906f, 0x6525d64066a888, 0x30408382bea707, 0x1330c34c891edb, 0x2f1fc41eb240cf, 0xc22dcb81d7da53, 0x54fb8904cb60ad, 0x68885f71e5b39c },
            .Y = { 0xc82988316cc813, 0xa21bd48a6c4707, 0xac46f3146668fe, 0xe8d44546dce7f5, 0x3856f476ab328, 0x38c333ef517fa5, 0x8a5ac457d4b410, 0x224ce1f7207756 },
        },
        {
            .X = { 0x7309ab2a46d83, 0x97fe4100602289, 0xf01af086de8118, 0x10a3ce73a144f6, 0x6b90329bafb02d, 0x1ff91282e43d44, 0x356c45fe14cf8c, 0x9f1bd94e08cb06 },
            .Y = { 0x69d4e4c7736616, 0x859916d0051228, 0x96ea848072cd1c, 0x91f97151976d45, 0x58e9d935d47ff6, 0x3d24d3b224fa2, 0x3d00ff3d70770d, 0x5354f6b8473a76 },
        },
        {
            .X = { 0x15953eee8ab83, 0x66fc377a58343e, 0x58b7a6aee835f, 0xf33a41d8555a24, 0x51463d48e20f9a, 0xbbb8bb4cec19dc, 0x8db0cb84888ab5, 0x83fcd61e36b129 },
            .Y = { 0x2ac5f6a451f882, 0x2b0422ba236067, 0xca6e2d5ccf025, 0xd7a8235424ea37, 0x1a7a6d0e90db42, 0x3890f78a27027e, 0x79606819d695c7, 0x8cfa5181bcb59b },
        },
    },
    {
        {
            .X = { 0xa6267111ccece1, 0x129e69417429bb, 0x58b8b2f6ec62a3, 0xd6335fa3fc1127, 0x5e95631411cac2, 0x5835f4a2fd18e0, 0xf32b376c4ebef2, 0x96534f82a1d367 },
            .Y = { 0xa0ad52e42efbe, 0xb0877dd9488d2d, 0x373877bbfebabd, 0xde0b8194735d80, 0xd8487542e1d47a, 0x6135bda64b0904, 0x4abddb239ffa27, 0x7ff94ffdbd4d95 },
        },
        {
            .X = { 0x75968a95729f73, 0xa71f71826ac158, 0x247605a2acf5d5, 0x31de3d0762f3df, 0x3a508979249b89, 0x5dc7c70d003b35, 0x6779230ec397e8, 0xc80607f19ba493 },
            .Y = { 0x1b05460790fc00, 0x3c22d74e4a1b35, 0x43cb4dd4ad862f, 0x7ae7a8290f492c, 0xc754ad89c6580f, 0x4fb5f23a401557, 0xe9e5a3ead98906, 0xffd230099af43a },
        },
        {
            .X = { 0xbcec2351bb0028, 0xcbb78ac967c537, 0xc13c20fb51caec, 0x48ebae9aaad0fa, 0x50438045a926b6, 0x142dd09edb30a8, 0xde1508abd1d14e, 0x885cf07bacc78 },
            .Y = { 0xf29dec7517e387, 0xae54f8d7ebde36, 0x12700bb23a525c, 0xee78d16e6d6215, 0xa206f696c7ad0b, 0x13d0ebd5dc4bd9, 0xac10177dee951e, 0x661f85ecbf752f },
        },
        {
            .X = { 0x9f94f0c501d7c8, 0xfc9c5030acc34, 0x9c5d11f86809e, 0xb6ddefc1e1cfe2, 0x427ffb805d9159, 0xae097872a1b12f, 0x3411de2c53f585, 0xc52de43f7c1880 },
            .Y = { 0xe977ab353376de, 0x6b5573f81c6ec, 0x43dfe434d53f29, 0x590d68cd7e7596, 0x60fbc917288a52, 0xbd7827cfe43b84, 0xee7a32a0057870, 0x201016ee58dbf0 },
        },
        {
            .X = { 0x5acb58bbb49654, 0xb92aaf617cd39b, 0x8c8bb07105abb7, 0xd549c74f1404e1, 0xd5a0cb09d6c89a, 0xce478b78c3b4b9, 0x1d092200a4e276, 0xcdc7d1ffed049c },
            .Y = { 0x13b3387d8d5401, 0x3f0ac8519bd743, 0x4b053953233515, 0x24929af5f0218a, 0x6b6156676d1f5c, 0x9a8c97b80de588, 0xb156baab5e0986, 0xbb6bfc254249c },
        },
        {
            .X = { 0x70359e565c380d, 0xbc00c0e81082e3, 0x93b9f92ee30cb2, 0x85e619b3de7b8b, 0x41ba5ef29a3ad1, 0x9b8f408fc04bd5, 0x67df9bc70fe3a7, 0x996cf652d7eba6 },
            .Y = { 0x237e5d5ed9dee0, 0xcf3f4544b0f655, 0xe0b4480f30a43d, 0x98f89d6f9d2ed7, 0x265eea0b1b5d3a, 0x41890c2260eb18, 0x96a65a313c80e6, 0xd6f42476d4f906 },
        },
        {
            .X = { 0x87cafc01360069, 0xfdebe1954145a4, 0x24eb7c1ef899fa, 0x5d0c5d17308dc7, 0x264067119a4e1f, 0xd5c26a0a0802b2, 0x97d05e2c818aaa, 0x19e71196407cd4 },
            .Y = { 0xbef94885de1c77, 0x582e4676b57d3a, 0xb552b77c89e0bb, 0xfcab328d18d23, 0xba3358aef84be1, 0xef02550f1d34e6, 0x667af548b7172e, 0xe391724f96d0ca },
        },
        {
            .X = { 0x53ce61eb526ade, 0xcc709fbffba43, 0xb761b100880ed9, 0xdfb446e7a62eaa, 0xc404fa6881b12a, 0xf1262bbaf6e1e4, 0x149df93b23747, 0x8bedfd992c2ac9 },
            .Y = { 0xd9cb0384f8b84e, 0x66f548f9f83782, 0xe6663ee6f7f161, 0x97b441e3f28cb7, 0xef8ef38df2a1e1, 0xa36ec6c4585185, 0x36a9a3f8c239fa, 0x46f6d0c7adba99 },
        },
    },
    {
        {
            .X = { 0xba5aaa4f2d1c93, 0x1bdd8c5c932554, 0xcf61392e9f9c8, 0x9ccf856c9dc9e6, 0x9ed6e189eeb389, 0x8893edabd48ca5, 0xc6e1964a8402ed, 0x78d923157c6010 },
            .Y = { 0x21415396c160f1, 0x7fe0ea4bcf3d9c, 0x5037bdb1c37bb, 0xfddaf81dfde969, 0x89c816f83015c5, 0x694c4039467083, 0xf1333ba5831d06, 0xc61deb7f2cb6d6 },
        },
        {
            .X = { 0x8bde208690b698, 0x232a2807f0045, 0x5882328c52adb3, 0x95b2a1b8684237, 0x3d34b018888100, 0x7256ea8f746517, 0x3fbe9d9e378389, 0xd5a3243bfb80f },
            .Y = { 0x1530edc3d531b5, 0xb8871721453b6c, 0x8a1028e5510c39, 0x7320cfcd3355d4, 0x65ea30ac46151c, 0x98f7d609809abf, 0xa2d9915058d856, 0x48d1753325c687 },
        },
        {
            .X = { 0xd61ac792a24bb9, 0x31410a80630a77, 0xdc797844023e08, 0xf9751d97b3a411, 0x846491ca32ffd2, 0xecdf42c078be7f, 0xeb33ded9eb1242, 0x4c9526242e6de3 },
            .Y = { 0x94453da1810649, 0x2bdeadc8e450e4, 0x82b43ca4f6b95d, 0xd9e233762e0a2d, 0x1d10fd08f85edb, 0x73a7ac322caa90, 0xefa0b052aa593b, 0x5d82d885142fab },
        },
        {
            .X = { 0x276e75d9b6fb0d, 0x586e39812d3d80, 0x9c92ace734a5f3, 0x9608abef3c031f, 0x52f3612ca32d4a, 0xce9c37cd8d39d2, 0xeb0b13a02cd8da, 0x31c54592ef1592 },
            .Y = { 0xa79f09fecabfd6, 0x9b874377db1bc4, 0x2183f1c4acb88b, 0x24863017a6046, 0x5fdc3814c9353b, 0x42abb98e5732d9, 0x8a3fc010723786, 0x98dd9bfc0c8492 },
        },
        {
            .X = { 0xb03bb4f40f7575, 0x30e1c36f93159d, 0x328d84a14ec1cc, 0x49810801d36357, 0x552dd382774273, 0x47c8668cd030f4, 0x647c6dd2289ffd, 0x61b1350fb2acac },
            .Y = { 0x6374c20021f6b0, 0x4831dd252d4eef, 0xaac9053a67a6d2, 0x9eebd56fa2698b, 0xf7ce6b93bdd9b, 0x66b55c28b09830, 0x5c0ae705b618b, 0x8620263f2be5cc },
        },
        {
            .X = { 0x38244a279ccf35, 0x22cf06a51e72b2, 0x8dada441bb0e99, 0x5516ead12005dc, 0x8a29822df7fe0b, 0x4a96e9f16990a3, 0xe3492db5db33d0, 0xc81fc08474f63f },
            .Y = { 0xdbc6115f0c6976, 0x2505d9cdde125f, 0x13e4f89f2d626d, 0x84f3b799998bed, 0xd52af3ee5aa49f, 0x9b18d8b12d29f9, 0x12bd17ff88bf61, 0xd8522ec05eb68e },
        },
        {
            .X = { 0x88cdc632174eea, 0x1216ad1390c67a, 0xb93ad9aa701d4c, 0x68003a83b6397e, 0x33ee162e1a1ccb, 0xad7b93bf2c1a4, 0x1cb6b59b62d336, 0x3c20fb1f562e05 },
            .Y = { 0x97260c38fb08f8, 0x2adcfd0fcae747, 0xf40c3dce7b4cdf, 0x3abcf3f75cde18, 0xfb383d343bc252, 0x36b7c2042f3733, 0x9e66bc683ba0b7, 0x754cda777199d8 },
        },
        {
            .X = { 0x1392a21df0bc1d, 0x81467a549407e, 0xc025d5d3219dd, 0xdd937d041b5c2c, 0x6efdb39a46f614, 0x42faa878905e1e, 0xfac69d7119e5b3, 0x2c9866eb2f7a50 },
            .Y = { 0x2527ced8c8cea7, 0x557c89854fed7e, 0xc9e765f31c2370, 0xd1d04e00ef7bc3, 0xdaac29e52359, 0xb534cb8ceaa2d3, 0x6696b2a9fc8749, 0xbbb97281e32921 },
        },
    },
    {
        {
            .X = { 0x7c62f613f357ff, 0x6322c9acce8990, 0x16e40a06475974, 0xef07f74bc616dd, 0xeef0f1de54b717, 0xedf1760d99611b, 0x8b5703ee90b6ca, 0x16c010b169be45 },
            .Y = { 0x3f991094a25000, 0x885c498f9cc4b4, 0xe295f2efb88e64, 0x9905f21b76dc59, 0xbd6762a4a580df, 0xc62ba98cc15636, 0xcb2da293806cb1, 0xce0ca3d279808b },
        },
        {
            .X = { 0x572cab365f93b, 0x9a6b2b7a7863ce, 0x9efcb9a844d5c5, 0x7a5a65846ae973, 0x90f255b6ff7fc4, 0xe5353d1337f8d2, 0xd8013c12fefd9c, 0xf787bd39fcec46 },
            .Y = { 0xded7155c521b00, 0x7011cc19376048, 0xbdb74eeaf112bf, 0xd684a9e4d0d69f, 0x243ade5b366158, 0xfaffe16d24c705, 0x206a251c9bffe6, 0x9ad252f49a9691 },
        },
        {
            .X = { 0x436718efc887bf, 0x45118aee0094d2, 0xf9e4b2e2fb96f9, 0x440bb4db43591f, 0x7df29cc9c2e428, 0x45216f94e7c052, 0xa3e6178fe0505c, 0xcd507f2a06fa8f },
            .Y = { 0x70e6f5b0954cca, 0xddb5f065e3fdb4, 0x3627415153fc4b, 0xdbba292c5818cb, 0x446f3c984abeb, 0x4c17562af8b60b, 0xb2d6b88ed76563, 0xf1f7224781af74 },
        },
        {
            .X = { 0xaf4f1b4d10a56, 0x1d95d5b8ee73ca, 0xec59b3ffeac174, 0x9df142217f0aa7, 0xb43af1d7bd7b74, 0xdafdd8739c2fbb, 0x1fbb40e42f711c, 0x448652f9f8b15c },
            .Y = { 0x9995714018e691, 0xaea4878a380c80, 0xeddf090c9def70, 0xb79f4ae2d07f7b, 0xd63085f77d8d49, 0x1e604c1034f2f5, 0x64dd266641376d, 0x6769537868ef33 },
        },
        {
            .X = { 0x5e19b4eed0e122, 0x4dd75b372b8bd6, 0x4454b003620285, 0x5801b1160103d8, 0xc7102e94f0d153, 0x5e4261fbc41163, 0x162bed65ab4fe9, 0xf43550b77e4085 },
            .Y = { 0x118398a7d8ed7d, 0xf277ec3b806e2d, 0x3f90bcd140231b, 0x52cba16077eefa, 0x9d5e4af6771f, 0x596ec6df298cb0, 0x26d25f6efd58b, 0x4769e8c13a9469 },
        },
        {
            .X = { 0x30a63c1dbf5ead, 0xb008bc55e06bbf, 0x5774ca285f2807, 0x563d04f0461ec8, 0xeff1a2e1f6076d, 0x83a3ff09c8a764, 0x9ded0f8860ed1c, 0xc4edfc3f754b28 },
            .Y = { 0x165d564242c0ff, 0x611b1b1e9fc4f2, 0x36283bb16b1832, 0x58ed0a3ecd8d67, 0x96f028428b2e18, 0xd99fea9a65ef45, 0xf67f5224a116fa, 0x9a41a1d9fbb43e },
        },
        {
            .X = { 0x121b91ce54d9, 0xcdeeb563cfbe99, 0xca337c7ce4b352, 0x88b59101152dd5, 0x61c6b35ea44788, 0xf438ff5e98d66b, 0x13a56547666cab, 0x824ba066f07013 },
            .Y = { 0x929cdb596bc164, 0x4902221b5c3a39, 0x7d52218b0fa2ea, 0x4df88e0448333d, 0x528a36c31c4c72, 0x7225efc8cf49b6, 0x914141e019b281, 0x8084dda5cb7c1f },
        },
        {
            .X = { 0x765d9a0f15f26c, 0xe0aceacd695638, 0xf163cefffd0567, 0x599815f830e98b, 0x122afdfc60a5c4, 0x7dc4fa368c998, 0x72be240bb2c9f7, 0x68cc51996da2a7 },
            .Y = { 0xb6a7d7b7681ae4, 0xecf72f8fee013e, 0x21f27a268b1dc5, 0x5b32d0222d133d, 0xac1d3fa8c1c3b0, 0x5b2107735966dd, 0x4b9394518bd983, 0x32ec932f8a09c9 },
        },
    },
    {
        {
            .X = { 0x4556b12f58bbd8, 0xf928319b1b48a2, 0xc65cfa0b2119f2, 0xd401134ad92b0f, 0x63d35c1231ca48, 0x4eeec189b8a199, 0x13bb0859b6c30a, 0x5101b3ba6ed3cc },
            .Y = { 0xf8069d54382d44, 0xcd94bf614ef266, 0x3ce9a53e9bcbb9, 0x872529475097e4, 0xca46c8589a022f, 0xd66d3818233a04, 0xf3ed058faf7d61, 0x36a54263ea193e },
        },
        {
            .X = { 0x77b9a78d2ddddb, 0x69eb21ef6f73df, 0x4baeb1f3ad1590, 0x65e85e34adff4e, 0x6fc23e16c09f20, 0x275a126ed6ba47, 0xb76e7aeea70658, 0x59635af5c269a },
            .Y = { 0x301a7fc52bdf69, 0x610d031cdebe4f, 0xcb525ecc62ec7a, 0xc45d37a89623, 0x7e3b1d13fcc24f, 0xc3c146628a902b, 0x4538a5fb2d8ca8, 0xadad9f3bfe4e73 },
        },
        {
            .X = { 0xcd0a40db80b6b, 0xd95d235189b648, 0x1794798abf7b3b, 0x64a85bcb7a6c00, 0x50db49fa98da51, 0xce6c4c5ac296e4, 0x5b8001f1febe85, 0xae42384f18b666 },
            .Y = { 0x470f71a66d3024, 0x197eaab05b54db, 0x9a7065ff4cd641, 0xa9caf9ba818ce5, 0x37ddfa269e526c, 0x7074d91784eeb9, 0xb357465a6cfa5b, 0xb4b642cb05d390 },
        },
        {
            .X = { 0x863b324662b984, 0xed76aeaa1d8d9b, 0x1120ba3a230ba1, 0x50f0334b98fff5, 0xca90deaeeb299b, 0xff37e64b6f53db, 0xa69359a1542632, 0xd17a9660b3d76 },
            .Y = { 0x8abc5fe42c1f4b, 0xe0f8e3a9ce85eb, 0x5c48449cdfce8b, 0xb9082e33841d5b, 0xd4cb69d586d5dd, 0xbe3254f22585c6, 0x7878a6da707dc6, 0x3a5a372a414bbf },
        },
        {
            .X = { 0xc6eac9831597dd, 0x6e2dd4d1c2f309, 0xfecc185a0c29d6, 0x50c197fffcf5f, 0xe515122b1e7d99, 0xba6cd1f7193093, 0x3a9a881fb7b164, 0xc4720bb2169dd1 },
            .Y = { 0x700737e91758a1, 0xad2eab5d830cfb, 0x23c5ebe4119e34, 0x9d3afb90c51902, 0x2d010bdda50292, 0xa11aa5fb6a67a2, 0xa7513a5e51555f, 0x21d152a4d763b5 },
        },
        {
            .X = { 0xe171fd74eb83ec, 0x4b40e00f813c41, 0x134883d083a768, 0xb6d2931eefb106, 0xbfdc851ea8a2bb, 0xe7115223cd429b, 0x46d061762e6a41, 0x3272f7f6d78de4 },
            .Y = { 0x4b7166a725e5c6, 0xe2290298ba8d62, 0xb304b142808896, 0xab18e712ea2b9a, 0x33c5c1562a1b5a, 0x7d279bd77bce1c, 0xb3aecf41bcd2f9, 0x5509c3f0b7d98d },
        },
        {
            .X = { 0xb8614398c5dc03, 0xec0b3b2e43dc5b, 0xe64441e1cf6673, 0x3e8567196ba793, 0x413f76117e14a2, 0x63240500fb4857, 0x6bc026a22e2d7c, 0xff67228826f716 },
            .Y = { 0xe581eac519a92e, 0x2548890fd41d31, 0x7acbcf60cd50f6, 0x7b9c51e2955881, 0xafc6120d0e837, 0x8f3e346af9795f, 0x34c930d056455f, 0x762328d19a5787 },
        },
        {
            .X = { 0x444ef3868d3420, 0xcac99d572819be, 0x1a1063e6c2cc47, 0x41cded3694c30a, 0xa6bb303a3bab71, 0x674e46aa37c23c, 0x3c8838486c50f9, 0x4831e235d77f79 },
            .Y = { 0x73b57029195773, 0x5a43afb2a52577, 0xc43ce9ec37e925, 0x16f32a90c1073b, 0xaf7690e55ded3c, 0xa2ace7ceaa9967, 0xfe17deb59e31c9, 0x2f534ebda7a396 },
        },
    },
    {
        {
            .X = { 0x6275fea30c9aaf, 0xf8e27dd9e2ae3a, 0x522a77e95cefba, 0xe4a6320705d889, 0x5aa5f2ebd26873, 0x69c61e56fe84ef, 0xf5ef818949e2d9, 0x542392934b2dea },
            .Y = { 0xaed3f189f65bb8, 0x37c1ad3533f347, 0x5c2c30e087f63b, 0x6967c4431deefe, 0xf112992f4f4cc, 0xf12eb33488e33, 0xcfaa981cb0c03, 0xa3e1b7b901e02e },
        },
        {
            .X = { 0xb43a68552a017d, 0x89ccf46a91bd45, 0xb3e0c25f6ac43d, 0xa22850c87f5289, 0x85783aebf15a70, 0xfdbb9c637d4409, 0xc7ad213b737424, 0x7a5cdf4f028f4e },
            .Y = { 0x6554d527f243fb, 0x10b9ec5bbbb913, 0xab0ec05ff0491a, 0x64dbd88aa2cada, 0xe88e97433bba23, 0xbc7b7cf7c386b4, 0x6fa8c40885bf04, 0xb06f674149dbf9 },
        },
        {
            .X = { 0xa2c76a01d7d87, 0xb84aed8bd23aa0, 0x579896971f49d1, 0x52be6198c4b640, 0x33c5a3804e8fe8, 0xb42326ec91b3df, 0x66ee65de580932, 0xd51fea8f3ef035 },
            .Y = { 0xe312f7cc5e73ef, 0xf22adeafc46802, 0xd46c5564535d1d, 0x336e629e2ee58b, 0x3efdf9da416889, 0x8f8c3c59c34745, 0xbcbf782a34aae6, 0x4671d8092e31ae },
        },
        {
            .X = { 0x3515a400ec3c5e, 0x17165b327cdf30, 0x14d9873a0877d0, 0x2cf39185fa5c4e, 0xa39d83e207d2dc, 0xb8bcc03ebb6f3f, 0xa2675d4bee2d72, 0x47ab7df7723208 },
            .Y = { 0x9f11807d8ce699, 0x92c64f53af4608, 0x9cd4f90d56b588, 0x69417faccf0da6, 0x4514840e907732, 0x855e775630f16c, 0x34e1ba13d150fe, 0xc58642e117ab72 },
        },
        {
            .X = { 0x3f1f9a845eb16e, 0xd256ddf7d2b62f, 0x669a6b7ffd0dba, 0xd0b7b43750f1d7, 0x325e7780dcf276, 0x1b0bd320c484f3, 0x5d95a6a5bd6a72, 0x9998208d82760b },
            .Y = { 0xa1e9dc1bc24730, 0xa1b3be48c67e9c, 0x3149a6d346156f, 0xcdcf23766cc8b5, 0x422b63a1ef70c1, 0xa7b29ba5001f16, 0x7d2224e0209bd4, 0x26e0994f224976 },
        },
        {
            .X = { 0xe2fc857b77535, 0xcd00f7026b9053, 0xed44e03dc76dd7, 0x74498b5f887a6a, 0x48620ad618eb32, 0xb6fb42b54ce188, 0x675693b216b8f8, 0xe5313921f70cc4 },
            .Y = { 0x94c5fc6bdd18ef, 0x92fa6806ddca9c, 0x3328cd8b92990c, 0xb4a6705aa1c5f2, 0x42e4963052542e, 0x871a8f9620819a, 0x1ae1b1e9373b4f, 0x9a5a8cf27ce14f },
        },
        {
            .X = { 0xf0e885488dd6f6, 0x730b06c1411651, 0x625ca2bc8da715, 0x106e6895f5467f, 0xec9e6f73bd30dd, 0x869f108ed2904, 0xc8d3cce59c5f16, 0x42561e12b5ea03 },
            .Y = { 0x2afcdbb68a82b, 0x26ad99919a62f0, 0xd410c57b918895, 0xcd45b10b0f80e3, 0x29297dd5eae2b2, 0xb52f5a3f591080, 0xce2398b3c1eac3, 0xb9f43130c0c13d },
        },
        {
            .X = { 0x590aeb6a69a00, 0x42037d8653084b, 0x69d9aafca78c7b, 0xe9dc38f502da3, 0x8867613168bcff, 0x5a7508f4c8f436, 0xf14b83774909da, 0x4ee69084c4d9cb },
            .Y = { 0x8266a701e482eb, 0xcf8c283107af47, 0x55b2a65243363d, 0x4e0aa3249ed89c, 0x2528313cc5ae71, 0xd990fcf71932ef, 0xab0c990d9690e0, 0x67029f859b99d5 },
        },
    },
    {
        {
            .X = { 0xdcbe45580cf29, 0x6bf1f79b57b130, 0xc71f8171b710b8, 0xe0b7eb1211630d, 0x542f2052d48b34, 0x604c0d45d7b0fd, 0x2174c07d96f3c7, 0x56dd6db4670be9 },
            .Y = { 0x89646608481063, 0x41071606930a0b, 0x499c69dcc62b47, 0x845d5745d7d00f, 0x97553c48269371, 0x8d99941184b1f2, 0x39dd669b024e8a, 0x8ee86f288f35da },
        },
        {
            .X = { 0xd7fa518cfa3273, 0xa2ef05073745f1, 0xae85cd05265ac4, 0x1dbc383656b82, 0x9d8edfbd03e0c8, 0xa79f99080bf78b, 0x64ee115d03f48e, 0x3efffce951159c },
            .Y = { 0x9eee0916189849, 0xb9e60f2ae812a, 0xdea6653fa6b781, 0x2a3847a2d6cca5, 0x49f0dd4aecd7dd, 0x7914a9fd8144b1, 0xe07ad9ceedd9fb, 0xef5f3335d18707 },
        },
        {
            .X = { 0x2e782174619cb8, 0x6b2e8838205d93, 0xa5bedf0c0b6d95, 0x6ff83f8286121b, 0x39b43642147f52, 0x9b2114b58f541f, 0x42de803efd4efb, 0x58122108393e39 },
            .Y = { 0xf3140a3d0c0750, 0x6ca1e21b3fe77b, 0xb44a979fe8c4f2, 0xe1d72ab4a6a6e7, 0x266968b2043b46, 0x3161865cba3327, 0xabe37ec4da842, 0xd7b058081a7272 },
        },
        {
            .X = { 0x1908cf4dbad6e1, 0xa7442b7559785e, 0xa63db4e882b126, 0xa9b86ed8a4a77b, 0x71ea98488c5113, 0x7d1519223dff17, 0x25e65c9a7884a6, 0x2ca3bc43ee7f07 },
            .Y = { 0xde0c7d6e38bffc, 0xfb0f842825a3bc, 0xfb5004b0e271d4, 0x38474106e51203, 0xeafe8bbd6ca470, 0x7e16f4d78e8b5a, 0x717ec8abeb1cb2, 0x4a77fca02bfe8 },
        },
        {
            .X = { 0x23a1e7f4e36b6b, 0x7b04d77bd93c4d, 0x45690c90b3e087, 0x25e22288422811, 0xc258b6d1e20fb3, 0x618aee5766263c, 0x7e363a36647d70, 0xea5f0249a27f2a },
            .Y = { 0xbc087a0c2d1831, 0x97ab05e4c53c21, 0x6bb2d817dc23fe, 0x1fbd456709e30e, 0x535a0b5353b3c4, 0xc75d9061ec96a5, 0x90805ddd36656, 0x6ad6d58429435f },
        },
        {
            .X = { 0xe4b85f196dc735, 0x7f10d119597432, 0x4d2e7e9eec4811, 0xd850640ee193df, 0x662261b9978a2d, 0x7ccf03843bec1a, 0x41de58b6a257fd, 0xa75a239ec3a789 },
            .Y = { 0xdcd5a21c0bcbe9, 0x6c75bd81eb7419, 0x55cc9abe27c278, 0x4e3df905a2f6d1, 0x42125dd45415ff, 0x8ff33affa2fad2, 0x37be0c1900af00, 0x52f151a17f264f },
        },
        {
            .X = { 0xd632c00f616a93, 0xc82d201bc5c54, 0xe75596395f3d0a, 0x2c3e7a2f0f8af9, 0x430a4be3cd941b, 0x442339485447e7, 0x37ec99d80143e6, 0x4e4e32cbb5a707 },
            .Y = { 0xedaa8e862625e4, 0x6614ac3933027, 0x2e769f003ad42c, 0x8ba4a314f4de74, 0x3b0dcd7518ab36, 0xf664bce91413d6, 0xb0083543166e07, 0x5185ae18f8acb7 },
        },
        {
            .X = { 0xc7d1afa736e5d6, 0x8340d142343390, 0x9ee0c1ec90de15, 0xdcb0cb3af39928, 0x61471320a09c9f, 0x57bad889fae19f, 0x72a3bab84f62e, 0xcec89ae9eb2f4d },
            .Y = { 0x220de179218a79, 0xf27273af254c87, 0xdcc3cc9b4547d2, 0x348400b9c899ac, 0x9db2bd345668fd, 0x55ff2e16c5e39e, 0x747cbf58febba, 0x5fb91f2fda1b6a },
        },
    },
    {
        {
            .X = { 0x3b9de8a7253e16, 0xf3857f51433c91, 0x57accfc239fcda, 0x43b8e6fa7e1776, 0xda120eb147ff1a, 0x55989fff71ccd8, 0xfee5246013c4af, 0x37fc0fdf5a695f },
            .Y = { 0xc65330502122be, 0xfa2a3ea365be8a, 0xab489f7d36d5f5, 0x13575f04c73a37, 0xb05ed9e1ae09ab, 0x808f951486d42d, 0x293e65d4f2aadf, 0x3f014640eb03cd },
        },
        {
            .X = { 0x17cd4d98dc9847, 0x2c87803b714a40, 0x86de6052db61c4, 0x32bba6acc4307b, 0xcf2103f7f81489, 0x78de693176558b, 0x8c9b4476f2a6e8, 0x9671f2e6c24075 },
            .Y = { 0x96c86a3b34d14c, 0xff881fbe10c4b, 0xdbef3f63d6de38, 0x873b8fde9c258e, 0x1be896fe9d71c6, 0x5fad8ca3aeaac4, 0x8c89be9c620d68, 0x65d13b78041162 },
        },
        {
            .X = { 0x57dfcd5bb73063, 0xb9bca198f970b7, 0x4e44fcab2d49db, 0x31965805bcbf3b, 0xe55f8a9cdf6594, 0x15df5ed609fced, 0xa3df0b0097aa73, 0xb9c28c6f5baa85 },
            .Y = { 0x17f6c3fc381915, 0x1ffb62b8d3794d, 0x2cea0a32eee1c6, 0xfe85ea97f3693d, 0x2d7565c293cdae, 0x1135311e71516f, 0x1bf283e4445d47, 0xf3554d66efd7a8 },
        },
        {
            .X = { 0x2cc12da544a311, 0xcab780d316ac3c, 0x1c7d4f199e962d, 0x6a68df5b1d8e91, 0x78c1f054503d66, 0x733317784ec005, 0x20b5054791d551, 0xd45af363d22ef },
            .Y = { 0x89bdd8bd0671df, 0x9f2a04d40d9741, 0x55ffa950c89cdd, 0xc5a12524e8f82d, 0x496797c4dadc68, 0xa17292418625e9, 0x7e48894cacb863, 0x3defd6cca55dff },
        },
        {
            .X = { 0xa72a203abe9c85, 0x9676bcf9388196, 0x9091e01bbd1c6c, 0xa0a733e638d85, 0x600bded5bd3064, 0xa4c6f879300f4e, 0xd4e62738c98517, 0x147af4a1b73737 },
            .Y = { 0x6b90806502c521, 0x26d8110fc05a64, 0x99f151e1b72267, 0xaae3b7b8f68f15, 0xa19f19f59198be, 0xcef51080c466af, 0x67fc32fe32ef36, 0xa8136f2a5c7b00 },
        },
        {
            .X = { 0xfad0621cbffe6b, 0xf2d39175db29b5, 0x1f9a78e594b1a4, 0xf509e96223b4fd, 0x38061f48b6ffd, 0x4e07bfb47930ba, 0xb4c560f6dc36a8, 0xa9119d42211497 },
            .Y = { 0xd7c69d4530833f, 0x9f816732a54142, 0xee5fc600e47ad2, 0x7762306c3b9493, 0x46c8644cff7a70, 0x8ce4e99d6be9, 0xda67b0d65a8e81, 0xf6ae81c2550393 },
        },
        {
            .X = { 0xa3618f832c4dc1, 0x27207b4caa669d, 0xd9700249223ea8, 0x9a2696c7b3cb4b, 0x9b1bffb744e542, 0xb858c7a7371882, 0xaa75ff604986f, 0xafd49e5afa512b },
            .Y = { 0x9db22997f2522d, 0xfde996a066c358, 0x40d621ce5547df, 0xe502bb9404cc37, 0xd2728ac605826d, 0xb23ef87e32e68e, 0xa214b459ee3ede, 0x382e410e59bce7 },
        },
        {
            .X = { 0x23f89df099c7a, 0x484ae12ae8978b, 0x5d433875c38864, 0x5d069d3a69b653, 0xae537d83e3be94, 0x1302652814f1e4, 0x7ffd27e5ac91ad, 0xb5170d6d895c01 },
            .Y = { 0x93cf73c72f04ff, 0xe8358f5b8d8f4a, 0xc598017f27df41, 0xaf3418aa2ee9a1, 0x6db87342a200c3, 0x29d475d8f4465c, 0xe3a123855c600a, 0x585b963e4a2751 },
        },
    },
    {
        {
            .X = { 0x1508a6027f0958, 0xb313bdc223847, 0x1ffe53940db75c, 0x7bb718470e35cf, 0x963ea20d4a17b7, 0x317ad8b368a957, 0xfe93e1d7fa68e0, 0xd88454c98bd224 },
            .Y = { 0xe5a4dc4b74ca60, 0x3358440f96323, 0x41c8d67888672d, 0x95c99c49c17ed8, 0x2db7b14ec41a68, 0x5c3c5b3054f16a, 0xc5d1c153855e6a, 0x7ebfb1ecaeb163 },
        },
        {
            .X = { 0xec387796357e37, 0x3fc73c9a5c4662, 0xdf0fec4b011e0, 0x7770e0132229f4, 0xa8ccabce8b91, 0x1e16392adcb43, 0xabef7346cfed96, 0x247ee05f80e668 },
            .Y = { 0x2097ae2751d86d, 0xe362499209787d, 0x196ef6b2352df3, 0x89ae2f1af0dafa, 0x1a3f6c9ce80618, 0x37722055c41dc8, 0xdb386a5fd4a6e8, 0xb4becf536a73e8 },
        },
        {
            .X = { 0x8fb6fecd064524, 0x6c8f237303d161, 0xb7c84b7ca383ca, 0x95e34e8647f34d, 0xc8d1a3075dc0fe, 0x28f6feac4bee9e, 0xe80106d809f35b, 0x54eb1966c25bef },
            .Y = { 0x2ce4bee79fccee, 0x1050fd47d5cb7b, 0x81a55b3a4e545, 0x5f5f5e34606184, 0xf174ab43ad27c9, 0x9dd6191f1605ab, 0x9a33efd086c6c, 0x9502718ae5fc24 },
        },
        {
            .X = { 0x4a48e804957501, 0xc99b8f4d2813d7, 0xc62285bc6e31c1, 0x607ac084a1ab05, 0xd38f639aef8004, 0xc577e368305232, 0xd5584b5d4ff125, 0x734ae56f16971e },
            .Y = { 0x87ebd1d4beeda, 0xb6d5e489c34c25, 0x3abe0b55dc0615, 0x3355f5d2f576f3, 0xfbe053c8da5666, 0xadf1af43dc0f71, 0x58b62444b14af0, 0x4f6b6e7b1660a3 },
        },
        {
            .X = { 0xc93621c543274b, 0xd1320fd8a45eee, 0xc962c87c8584b4, 0xee7dc9a9c0904c, 0x98daaf2482ed59, 0xabb340f693cf74, 0x71771f3da849, 0xf09e55cdd6256 },
            .Y = { 0x32a070e1f9d9, 0xb62bcac235ef8, 0x8ae834aab806b8, 0xde97f7a84379cc, 0x4001c29c3673db, 0xf8b77ff0b81330, 0x8cd78e6c342b2c, 0x6419defd06df39 },
        },
        {
            .X = { 0x72bd63960f7f09, 0x6f0175daa3b10c, 0xa3c10366ebce19, 0x18934eb6658cdd, 0x53d3b5bbeae70e, 0x7b102984d526c7, 0xed2c3e852798c3, 0x39f9f919943c83 },
            .Y = { 0x3b03962b32cd4d, 0xcd354076d8e18c, 0x8d76c66caebed0, 0x2143469c8a16c9, 0xe11b91d3bd0e7f, 0x4d59b4a7f4a65f, 0xcfaa2f972e1ead, 0x2233aa451dc2be },
        },
        {
            .X = { 0xa1575c8fca2313, 0x6c332068f52148, 0x514dfc74abef2a, 0x7be462aa35b629, 0x27b19eb4086137, 0x3f3e4adde49742, 0x9b773054865b1, 0xb026e4872bed9a },
            .Y = { 0xcbb5ef47c1a61e, 0xc0bc3a8fae0c4e, 0xbb3ca2a39a14ae, 0x6743cec25f9781, 0x4103ebd4b0a129, 0xdc66a2085fafc3, 0xc4ff130da08601, 0xc9ab313b63f6f8 },
        },
        {
            .X = { 0x7edccdedcd5395, 0xa38a81bc70042f, 0x767b298765e022, 0x983f67d415cf77, 0x6568afb026e649, 0xb5d0d19fbc95fe, 0x50829e54258256, 0xebab1e8bda40df },
            .Y = { 0x3c403184563513, 0x8ebf1e41289dbc, 0x9812cd25e88b47, 0x2c4bdd67d6b105, 0xf93d9d95fd7e20, 0x3a66daa932eba4, 0x25e2a3dc9a84d7, 0xc633335c529d66 },
        },
    },
    {
        {
            .X = { 0xc5d65ebda39266, 0x3a1bb196ca3ac3, 0x5110ea09d7ae2a, 0x2c8f038b93b47a, 0xc5b5f5aa1ec480, 0x59df9138894b3a, 0x90da25d54d322b, 0x61901059f3c58f },
            .Y = { 0x6ec6873b7ba30f, 0xe42dd80612fbd7, 0xa1aa689721d944, 0x8efb3330f39e4e, 0x777df53e808429, 0x20d2d1b9c3b2b, 0x711f5b5e7a5adc, 0xabe36ef6e517a8 },
        },
        {
            .X = { 0x48989f7e67e47d, 0xa07816c0c85105, 0xbd162c893d7fa9, 0xab9bfab9f0557d, 0xd41c5e4917b321, 0x7a4f689f05faa2, 0x5b9012daddbc89, 0xdcec31d092db5 },
            .Y = { 0xa6ddc4f8fa48c3, 0x113c7c3aa3f34, 0xea38eca110458e, 0xc08d8cabcda4a7, 0xb048b3c58d9e4f, 0x3304117220aa82, 0x3aad361e921e13, 0x330cca3e17d9e3 },
        },
        {
            .X = { 0x97240d0be7169e, 0x74390643b76a33, 0x18d1d8a497917f, 0x3429c9837df2e5, 0x24db59dd0e39f6, 0x765c2f42d520a2, 0xca980e069d107c, 0x91249b3d54c0dc },
            .Y = { 0x6fd66dd9f7ee32, 0x90b6c59f0955ae, 0x88c1cd37ad0789, 0x1bc077d9b6e895, 0xfe6094c476e624, 0x5a9c1566193c45, 0x72a0733f0fa5c0, 0xa47d248cf99bd1 },
        },
        {
            .X = { 0x2f6d2c8e8a36d9, 0x4f448311ce030b, 0x58fe8e83140446, 0xf8c159184c1fa5, 0x3be3e85deb1cb1, 0x84b7c40e53dfdb, 0x7f11bc18defb31, 0x6947dd07a92f70 },
            .Y = { 0x115262d07d8a21, 0x52e3ad116f77ea, 0xd26d14e0ffaae9, 0x8d31e19d93a82b, 0xec7cec2f25d431, 0xd43ba52b758de4, 0xf8384ef01aaacd, 0x26344154c63b0 },
        },
        {
            .X = { 0xf01bddfa60bd2b, 0xe60294b5c520b3, 0x18cf8cc3f2c56a, 0xe2116c4dd18f34, 0x121c0b710a2498, 0x18eba6dcb62867, 0x3fe9d65311d120, 0xf135ca4ad7d062 },
            .Y = { 0x39af055dc540cd, 0xd073cdbce7e321, 0x69058935ad3422, 0x61be90f808a407, 0xa13e77d8a2c066, 0x5a2e1bf993c040, 0x75485cbd84f760, 0x6daf1d75ec6aa5 },
        },
        {
            .X = { 0x60ed9497f8a2e3, 0xd1b7d062d6ecad, 0x913fd35e8d50c0, 0x5b65012c03cde0, 0x14699d432480b4, 0x67c995813688ea, 0x1d7e84335407bc, 0xf02a4a34219af7 },
            .Y = { 0x61fe1d5a72a945, 0xd9751e10d39e52, 0x7342d9a94198af, 0x1fac18ce9b7ab8, 0x387d605eff8ad1, 0x7c1ba9838a0c3e, 0x93f71805661a7f, 0x9d65f617180da7 },
        },
        {
            .X = { 0x260dacd50e4d63, 0x882b5edec3db46, 0xd263883ad8e7b5, 0x6c228a48834121, 0xcf7ffc8e0e946e, 0xe40e7cc0991f88, 0xc75ea850247368, 0xc0485452f41e5a },
            .Y = { 0xe7243fa0670c3d, 0x52ec307783db4c, 0xfa494f6e5cbfd, 0x428d3c549271b, 0x6452116f8b32d6, 0x260e7482b76d8a, 0x8e50422bbfbb19, 0x16b00881340418 },
        },
        {
            .X = { 0x331716cb636821, 0xef5ff7672ab642, 0xb862f22ebc4602, 0xd53a04da71cb2a, 0x7d035f048f9963, 0x1e7c3c74ad5058, 0x84edf3ff69230c, 0xa5324c455d4144 },
            .Y = { 0x85e83cd372694e, 0x6089b0329f264e, 0xf85116090b3f0c, 0xb638ca5434501, 0x66eb8761d97dbf, 0xad669920d6166d, 0x7292c35c10c88b, 0xa3880c6c1c6dc7 },
        },
    },
    {
        {
            .X = { 0xcdd11f6ad6cbf1, 0x9b2e598ea3209f, 0x6a4b3b11a3db90, 0x65275314d258c3, 0x840cb4d2db62a4, 0xff32ae60974d57, 0xd48494af968ef, 0x475e51108f96d1 },
            .Y = { 0x8f8c5ae9db5386, 0xe4b3eaf596da, 0x992a4b98a567a4, 0x6d082338f3b446, 0x7cb507f6a7d69f, 0xb6fb645c49552c, 0xa15fa3533e87f5, 0xd93f318debcfc3 },
        },
        {
            .X = { 0xdaa33e2e783f12, 0xe4bdff3fa4142d, 0x8ff43042b26b43, 0x3ced28e93d8eb2, 0x9f8a2cd14d543a, 0xb1aebb41bdc9d0, 0x2c5ea8a8921efc, 0xad0a6b86dd47f4 },
            .Y = { 0xea038f33ea4bc5, 0x10c4eb9eaae269, 0x8d19bc5a7c1bc1, 0x208639c5ef4a79, 0x263512e1cb62bf, 0x63da1fd486b615, 0x909eca2103c98b, 0x8be17815a0d1a },
        },
        {
            .X = { 0x49745b40bec8d8, 0xa2730451e6e7ed, 0xc40b42fdddb8d4, 0xf3deb71f98dd81, 0xd47b9ebc68cd23, 0xeae732e1530e86, 0x5a58d58ebb8dd8, 0x82901cfe1bf955 },
            .Y = { 0x7a110894b86525, 0x56f8c2c66d347f, 0xee6d42ca20773b, 0x41e1c741b46c15, 0xc7cad011049ec9, 0x48066f6c494cd, 0x9b59530a2ca6ae, 0xdb28b2f41c1455 },
        },
        {
            .X = { 0xad3e2ca93f736e, 0x56e253b45138ef, 0x3129fed135fd7a, 0x5961d88b440419, 0x295f8666398511, 0xd1261e18decb9d, 0x8fcf97547f6c1a, 0x7b124040c20179 },
            .Y = { 0xe5792dfb2d465f, 0xaa3b91ed68a7b8, 0x783640c9579d18, 0x7b973b1f674a7a, 0xaae99bcc99a310, 0xeab7ab18f19a44, 0xb49d42ba7f75a3, 0xe85290b743c5a4 },
        },
        {
            .X = { 0xcd0208194789c7, 0x58636b5c8ba02, 0x66e28e2e0e25f8, 0xeba5830ead099b, 0xe5321abd4e9f3a, 0x69913a8013a362, 0xb970127a52bc19, 0x5d408502f069cc },
            .Y = { 0x42b3d0cc27ecf5, 0x96cb3819fde875, 0x3549e8bfa42b51, 0x2f419119999edd, 0x7532bb15fbab08, 0x8f7e52bb20e5cd, 0x458ecd57123303, 0xd792643a6d8b3a },
        },
        {
            .X = { 0xd1e8abd57b5387, 0x7cd85f1ea11f9d, 0x289e5f97160757, 0xdec6ad7e00facb, 0x98c06d1a73c61b, 0x10f80a4b5e34ad, 0x782341c5044f32, 0x325b57210626c4 },
            .Y = { 0xc290d06f3e994e, 0x87053e51b1b3b6, 0x6b9721b3c7a4bf, 0x542e9dbfe88d4a, 0x77d1f80aa701b3, 0xf0d631e11a3393, 0x7ec4b67872926e, 0x97187f0f20d82b },
        },
        {
            .X = { 0xe7edc07c53b9d, 0xa56f2c62d78e08, 0xb2beac9dc45c95, 0x9b3ff3f8ad128e, 0x450cfad543c851, 0xcb7d9619585ec3, 0x411158df99fde1, 0x87c2b292270b5f },
            .Y = { 0xcf27a633253d10, 0x92f5f57e349bcc, 0xc9794f985cce02, 0x2f61a33035f0de, 0x247cf93d8a7cab, 0x6295203f0ac3f3, 0xcf34c5a18bbc0b, 0xa1ca521f7ebf54 },
        },
        {
            .X = { 0x4ad2efd60519fc, 0xa4c59a472bbaf3, 0x723710a2712ee7, 0xd6e45b0724ca74, 0x83066a9a531944, 0x4fa96d004d44ba, 0x53b661dfb41bc9, 0x675262204e2252 },
            .Y = { 0x3e01ede0f83f23, 0x62bef120a71f17, 0xaf18c85c2e43cd, 0x5d16ed2039fa30, 0x837b4f3fb871f1, 0xf898c655dddf2, 0x61200a8c17e4e9, 0xcdaef81dd5383f },
        },
    },
    {
        {
            .X = { 0xa2ee46d6ca820d, 0xcf95bdb41ba489, 0xac7c6384eb2f73, 0x66d1d19090d62a, 0xbeb3fdd5655deb, 0x2fcb6411e6a2be, 0x36e1dfba34589d, 0xe1b6556d4d20d7 },
            .Y = { 0x2f3cb006306af, 0xaaf0ffeefba840, 0x929ffe288b564b, 0xa843841f70537e, 0x46cf5f70259c53, 0xda293392510326, 0x78b6ebcd803d5d, 0x69d2fc1c0d2778 },
        },
        {
            .X = { 0x6748fa7e608675, 0x96b8aa0c11c084, 0xde6c6c571feb19, 0x6be21e3bf02352, 0x80dd07067c42ae, 0xbcc31be5aa1271, 0x4a87265d143e7d, 0x69c802ea85f2ee },
            .Y = { 0xbdc2c44ee93b30, 0xbe41bad0234607, 0xb0fd2719dcbf4, 0x3a52c23720bcbb, 0xd7bd32811ca406, 0x5a14866581ff8c, 0x4c4dc3b1968122, 0x8c8987c4955402 },
        },
        {
            .X = { 0xb3c00ba13de7b5, 0xb384c4bdcbee9a, 0x55e0d3e6eb5dc5, 0xb12d50a7024bf6, 0x9ee77215f9b974, 0x4acad265c03a61, 0x4f0db686451a37, 0x535754ad43b1da },
            .Y = { 0x135975eb114eb7, 0xf4e97a9f05002d, 0xf4f529c6edfde2, 0x868d3eecdb064a, 0xa30127d5ae7c5c, 0x122aab48348894, 0x75e895f7e53915, 0xb5e81fe39cc87c },
        },
        {
            .X = { 0x5a3e7c6a009547, 0x798099ad5a0462, 0x56c57b467f3258, 0xc93f917744bf03, 0x711c672829b8ef, 0x211df4159d7712, 0x324583df94e47b, 0x833be500801e3d },
            .Y = { 0x7f3540e6d7462f, 0x4ee27af05dfa32, 0xf54db56078b64b, 0x9df5a5accadd85, 0xf2a8ab7345fd61, 0xdd15757d71b835, 0x9ff63fafac73d3, 0x16b7cc1a30461c },
        },
        {
            .X = { 0xaa59b53c389236, 0xecfbd14d947970, 0xb04e5e1f7fce77, 0x35c645de0d6756, 0xbeaf5bf8970400, 0x65255a9cf62906, 0x9c00637424588c, 0xaf6c763606e490 },
            .Y = { 0xd2062036d59b50, 0x7d117eb775955c, 0xcb5b0081e888f4, 0xb29a0ae6e7f2c2, 0x240ea3998b8200, 0x5411642450373d, 0x8115b1c65cdf6c, 0xbc808119302946 },
        },
        {
            .X = { 0x36b4d280e338d1, 0xdbcc35dbe1dbba, 0xc04f739642fcab, 0x69350078ef1ec5, 0x20455112a9015f, 0xbf6b84a8ac41a7, 0x194f07d7881bdf, 0xd678dc3a360a1d },
            .Y = { 0xdb77d947cd81f9, 0xc9f2bc966e10fa, 0x32cc3e10f64e83, 0x9056a6e6c9c35d, 0x7086ceb8d130f5, 0x31e02df82f3e97, 0x93aa9e90e7975f, 0xbf85fca5e012d5 },
        },
        {
            .X = { 0x22f876c3859ee9, 0xda5a4c0758f4b5, 0x4de22dc57a8e86, 0xda8bcab88b5d73, 0x857c72497b747e, 0xedd27b2acb92c1, 0xd670a778b2cf0b, 0xa5a18cb492f0c6 },
            .Y = { 0x779dd1bdce3a05, 0xca22a42d9e76d8, 0x2bad3886976b7b, 0x5f518205309e25, 0x607437a480e165, 0xf6250395e94dc2, 0x2b87d85c575646, 0x94b380a9230405 },
        },
        {
            .X = { 0xf29c42a4ef55c4, 0xeabdfca5f1880a, 0x86f117566ff994, 0xaaa25e848eb468, 0x66c576ff7b8f93, 0x3e3bd547ce90ab, 0xb423d7cf4c0836, 0x35ad7b49ed0ce5 },
            .Y = { 0xd1d1eeb414049d, 0xc6c6284daab3cf, 0x33312218f209b2, 0x95c31f11b0b35d, 0xaa8d10c842fb7e, 0x930088e3ca9773, 0x44f495ea873ca1, 0x84363d91d94541 },
        },
    },
    {
        {
            .X = { 0xa87e59975d6f43, 0xe6658f075433a5, 0xe706b69db37f79, 0x1841a76327a724, 0x7f8acd810914f7, 0x82e7f0919e1502, 0xd0b586386ba333, 0xbf04043a085005 },
            .Y = { 0xe518c8887d90a4, 0x35cb23713fa568, 0xab1a26f91f7f79, 0x5a0038823bf987, 0x455f13a9fb264a, 0xf961b0345a3873, 0x6fb7bcabe1bd41, 0x5580bedb54e37e },
        },
        {
            .X = { 0x5e889898ce090a, 0x4d14a2aa5c4799, 0x1e8f4e88c43c61, 0x88aafc52e42352, 0xf31ec2661699dd, 0x8c8bf78d17fe14, 0x2c7274bb92a4e4, 0x2fd471e0fc85a8 },
            .Y = { 0x3aec7f9e3477f0, 0x2d59a295c4f9ae, 0x6e9c27ff20a2d0, 0xff2f0c3ec53e86, 0x146bde1cb8ef56, 0x42c04aba333fc9, 0xd5f5e8318d97bd, 0x657687cc0ca61a },
        },
        {
            .X = { 0xbdefa42d333042, 0x6786f1c6f469aa, 0x18ca12b1809bf9, 0x61af85b298e3be, 0xf14b866f96c79a, 0xb1f16b41d2c201, 0x22a86405fe2e16, 0xc1527b8f63bae6 },
            .Y = { 0x445fd88dd49d6a, 0xaec56d4e1cf39b, 0x94d8fb7e482b19, 0x194498bc49a86b, 0x7daf212ac40b9c, 0x65d6eac604dc76, 0xf180979796a2d, 0x21f0605a3d7e23 },
        },
        {
            .X = { 0x6d227fd8268c7d, 0xa05eab803233be, 0xc7913a5c51f4bf, 0x6ccb35d953f906, 0x32efd25d0a6a72, 0x4bfb87fb04a569, 0xa9de62cc98cdea, 0x8e570fd336586b },
            .Y = { 0xa81a9380666301, 0xc6292513e859da, 0x58014f1b000f73, 0xdb6c45785925ea, 0xc383532faf2584, 0xbb6c3e1c728887, 0x2d095366c71707, 0x89bd6441d61849 },
        },
        {
            .X = { 0x297f0cc8631b57, 0xd298b3e1ac4046, 0x6bdc9c0e5ef689, 0x37f4cabbcfffc1, 0x5433fd58ad42a6, 0xf57d11a8e245c5, 0xe722addb1c11f0, 0x22469f116619c5 },
            .Y = { 0xde125d80d45ae1, 0x3cbe920135a8d7, 0xab31f7e28e2054, 0x2200865148357a, 0x635b3f6e135d6a, 0x4a71eb2e22261, 0x1ac976aedd6381, 0x40837b339f314f },
        },
        {
            .X = { 0x83e41cfbefe1a, 0xbbd0ff220d54d7, 0xee43fa70a96d4b, 0x867298a0134f6e, 0x61502039d7ac9c, 0xa0c863ce762853, 0x820aa642684698, 0xe10ae83ec221b },
            .Y = { 0x28ba2370aac20, 0x2b2f9f9b8f753e, 0xa033e2a3ec54cc, 0x3effcb6dadedb, 0xfad4f23abf1b7d, 0xe20110b3109bf2, 0xe5f1a592e9b3, 0x67208bc6fc6e2e },
        },
        {
            .X = { 0xd1fe448209bfae, 0xbd890e68a3d066, 0x5a98d5795ef057, 0x38db56fb436f20, 0x775f379aab750e, 0x7bb184138fe0aa, 0xff85da8c93ebb1, 0x92c8e12ea8b141 },
            .Y = { 0x55ed657095b0bf, 0x28a77e6e91160c, 0x1cc17cf5770fd8, 0x4d7b45d108a801, 0xf581495157d02b, 0x9cce07a5c04ec8, 0xb98c767f44a8e3, 0xa9117ac7c48a70 },
        },
        {
            .X = { 0xa3cc03d6a36a64, 0xff3df336b4ce5c, 0x80ddc786fba561, 0xb50786a74e46b6, 0xdf84536cc7be3b, 0x33755f40542098, 0xdff39e5bd69e7, 0x1e23de5f50eebb },
            .Y = { 0xaecd34d93aa177, 0x1d290772e30981, 0xad96d40a3e0d70, 0xcc8e8d40f95aba, 0x592d1154d8eda5, 0x1e9918feffb009, 0x556ebff1aa93fb, 0x86d900bfb217f9 },
        },
    },
    {
        {
            .X = { 0xe29c1b9840bc87, 0xa28bbc0002b446, 0x82e8c5838c2d23, 0xc3162be420cb1, 0x335242ffab3b14, 0x338750c7bd74b, 0xded0226a48ae10, 0x40c2b4081ef720 },
            .Y = { 0x1f2f0df71d993b, 0x8af7b6cf23acd9, 0x9ff776164428a8, 0x83b2d882358571, 0x5b6ea9d685c922, 0x157786a41d61e7, 0x418772e73716f, 0xe4742ac3fe41db },
        },
        {
            .X = { 0xa1e92bd6b66316, 0x4c5fe2ac58e982, 0x720a9e2bec19e2, 0x18584623060216, 0xc8fec84f568da5, 0x3ba563f1a80b59, 0xb18eca182c22cb, 0xfcb0b3534fb8c2 },
            .Y = { 0x485ea8526da812, 0xbbef9ed78aa012, 0x79f758daa9ecc5, 0x136bc1ab3c94a4, 0x79f0b233a88ebd, 0x32f65889397a1e, 0x942ab0b340e129, 0x20de8415aabf34 },
        },
        {
            .X = { 0x20f21c3129da22, 0x82f24104e26dc8, 0x2db77a817f8f51, 0x11192fa374b02, 0x30fe9944d67ce2, 0xe5a11f12f27474, 0x581a7d71163073, 0x767220f794b1bb },
            .Y = { 0xaec9965635cf2a, 0x398324a179067e, 0x166c38a142be87, 0x651b38e4f787f4, 0x4455e6e404c3b8, 0x41dcc90a4e1c3, 0x342932fd7d1c8e, 0x4f863fbb254ce5 },
        },
        {
            .X = { 0x928b88b0097416, 0x2032d6123055c4, 0x60e3ebccabcaf0, 0x27efc99b042efd, 0x59375b59b7540c, 0x8908a26456da9, 0xe2f263e873a0c, 0x2cce7587f2192e },
            .Y = { 0xac1bfb2b83197c, 0xbb44c329cdcac0, 0xd117ea37a53080, 0x6a6175dbf45431, 0x37035fa249fff8, 0xf7e547cb8b321e, 0xd81469e0ceca7d, 0x1aa647ed22821e },
        },
        {
            .X = { 0x1f42df4f129005, 0x845d2a60cd3b76, 0x549b329f8c0bad, 0x60e092913e125c, 0x6c14aa6b864a8c, 0xfa17b2b3bd76b4, 0x2acc3769bbd154, 0x19e799c4a68464 },
            .Y = { 0x2036fbf508e9a6, 0x66f85d96d9e54e, 0xd152f3abe8ba84, 0xd3023bfe0cadd2, 0xc6b313211b43d5, 0x806a60e6fc8140, 0x672d6e4afa52f0, 0x9abcbe72e89780 },
        },
        {
            .X = { 0x7bcacf715e1553, 0x13bb585e2fb124, 0xc0aaf146646b62, 0x76bb5a63b43b24, 0xb72308e604c1bd, 0x2c385e4b9edd9d, 0xbc7252ed85261d, 0x53dc4ad522e7fb },
            .Y = { 0xc641ba261c5672, 0xae794d111515c3, 0xa0bd73aef7609c, 0xf0ce39964835f5, 0xf6f75d20fa2b83, 0x1f09f29fe5fd0, 0x72dc74b5873c4f, 0x9bc850ee8438ba },
        },
        {
            .X = { 0x44d08758328fdb, 0x86c9ca54baaabf, 0x8292020c14d77a, 0xdb876053c33dda, 0x6bd21458a65256, 0x75a2f2cddb30a8, 0x7f607948004a70, 0x3e985b3c19e64e },
            .Y = { 0xd2a5355075f042, 0x83bd9eecca774d, 0xb75b0ffe6229a3, 0xeaa158a51c3035, 0x27c55c98cafb50, 0x65061e76e20545, 0xbfd6f829f93d09, 0xbda5295023907d },
        },
        {
            .X = { 0x7ae0e50eff5feb, 0x1fb2811789e2a8, 0x94058ec9e5fe52, 0x39423720a810ed, 0xbc9cdaea948b, 0xa3043ed77aebe8, 0x8abb3ae9a73ed1, 0xa9a5d7e0d5b53a },
            .Y = { 0x8736f4f0617489, 0x201c6415df6513, 0xd8ead1b80808d2, 0x8cf3d307cb050d, 0xe76edc2072f145, 0xe534e4cc240417, 0x5e0c02a4d75938, 0xad9ba86562cd87 },
        },
    },
    {
        {
            .X = { 0xadaa878c6beca5, 0xe49da15c3e88d5, 0x18f6437df131e, 0x4282261b3843af, 0x2cbe6873307a71, 0x24cac5b2ad65a1, 0x420527a3680fdb, 0xb24c330b5ac6eb },
            .Y = { 0xb0af865a2e0890, 0x35ac0404a15d43, 0x8e0017014cf996, 0xad5f92c762c010, 0x2310074106e111, 0x7fed0e07aad90e, 0x8d76bf509ed9a0, 0x27dbfe7a67d427 },
        },
        {
            .X = { 0xa37746146b9e02, 0xc0877863b6c47f, 0x4cee2f33971a78, 0x992fee65225db4, 0xc61a88659849a5, 0x694e8ccddaaee4, 0xb5723c892ce2be, 0x9a72bdf84c0eec },
            .Y = { 0x82b42bda779731, 0xcd1b51085afe32, 0xb708eaee20e7f, 0xc80e0b0c716a2, 0xd49de7bd8f9a3a, 0x8d955ce974bfb3, 0x92ae51b5fcf3aa, 0x59da889ea2d7de },
        },
        {
            .X = { 0x833ac6df443c71, 0xe67c7a580045e3, 0x5396d46e03696c, 0xf216ad7fd10d8c, 0x17c93b5dc306b0, 0x45b18cd6415eec, 0xe7835020757fb6, 0x84bab4df0e7b8e },
            .Y = { 0x2e07b555c9167b, 0xf689e50bbf9bc2, 0x7be77520b91446, 0xf083a615cb4ef3, 0x1b5c0dd2d80296, 0x6abe159fa3df8, 0xa2c63ec244b940, 0x2eaad3a1dbe865 },
        },
        {
            .X = { 0x5cbc8a1f8853fe, 0xb6aba1026f5ef3, 0x628a601526d057, 0x7417404742fa0b, 0xf94970c27fd8f2, 0xf67bdb54b4a2ea, 0xfbc9d8c7756b54, 0x7f244f7a675db7 },
            .Y = { 0xbf6c4d76b108d4, 0x8b26d9c2b3674e, 0x30091fb44f21a5, 0x11428b2d24f242, 0xd23b055d116f60, 0x4bfab01bfcea35, 0xedad9c9d41ea8a, 0xa920cb928d5ba6 },
        },
        {
            .X = { 0x2f6c86f6a74abe, 0xe583af2d880401, 0x428a6fbf23bfe0, 0x79d9ee234bd76, 0x4e102913bc962, 0x2edf9c24734a8a, 0xfc8f78c6a67ffd, 0x2caf2596f7a633 },
            .Y = { 0x9befa2570b8fcb, 0x292e0851f284d0, 0x473199eb67a019, 0xa1e247f05e1893, 0xd109d9f19269c4, 0xc56bdc087a9eb7, 0x4c3ec2e44fc4cc, 0xfe3a09ca41a4bb },
        },
        {
            .X = { 0x41cb784500dbcd, 0x4979cd413fd9d0, 0xf1ceb3ee47a8c7, 0xc49c4857a418b7, 0xc1e6b04c6ecb33, 0xff378eab1c08e4, 0x9223598facab56, 0xa1b6f71157c469 },
            .Y = { 0x8f1cfdae70f59, 0xff19d6da80a7fc, 0x774c9f8a8453e1, 0xe8644c53243af8, 0x8917df0aebe02a, 0x53d5d6239d2647, 0x3ec21db4bfa97d, 0x82196adb65c85e },
        },
        {
            .X = { 0x7d85eba5d3f232, 0x7449d756f993ad, 0x99ac6e4e7988f3, 0x9c86bbc5a8cbf8, 0xee2b535ab6c565, 0x14c0b5e5dc19b2, 0xc2c82e9daf36aa, 0xbe31b0d0c68e81 },
            .Y = { 0xeb3dac18729652, 0xab3784012468bc, 0x54d9d99debb8ec, 0x10b665ae64e95a, 0xda2780925865e0, 0x4aff52e158bf10, 0xe15149b50332dd, 0x4f292fb4315408 },
        },
        {
            .X = { 0x1b605751e1e9c9, 0xe6b0e9d23625db, 0xdcab78b42e2a00, 0x514e2af322c60c, 0x690d226160811f, 0x1789b7fb76f19a, 0xea6e5a80b51290, 0xc3e7c3db120ddf },
            .Y = { 0xbad6682a2cef9d, 0xc0a683563a837f, 0x43349958eaafbf, 0xf21b1cf33772a4, 0x17b807b15a1965, 0x8ce0ff5ced1bcd, 0xbc58dfe2e7b9c5, 0x8f65a4acfaf692 },
        },
    },
    {
        {
            .X = { 0xb0c21b9813efdf, 0x9ceb3cdd92a8a0, 0x93f9f88dbaba6f, 0xc1d698622e5156, 0xec653f4e9f6138, 0xc2c614de5120c2, 0xd3eb73642dc761, 0x3043b244562f53 },
            .Y = { 0x8fd43cd457482b, 0x32819cf3577ff2, 0x995576d9eecc2, 0xc3dd45c84c1215, 0x5c4926f1371289, 0x25bbc0f106a065, 0xb316e8218344cc, 0xd4ce2e25439d74 },
        },
        {
            .X = { 0x88cc6c42e676b7, 0x56f84c4e82a5c3, 0xafef8fd90d895e, 0x7730a073886b51, 0x1953d73659370f, 0xf071930aea33c8, 0x85820267e688b6, 0x5a0321b5ce995d },
            .Y = { 0x1ca4f70348ace7, 0xd0364ea7476ef8, 0xbe5c714e3dd3ad, 0xe2a0c9b5f01d65, 0xa4bd5ea4707110, 0x5baaebb9f6b849, 0xe130241b7dbdde, 0x37cb25ab1f91e9 },
        },
        {
            .X = { 0xcd761d8d8989ec, 0x132119692a348d, 0x914d6b71a7cdf3, 0x16701c9343e45f, 0x25e223c307c81e, 0xd112073cf97cd1, 0xd53a98e6d17bc7, 0xd487de896f64e8 },
            .Y = { 0x5af7683fe3678b, 0xde594b2823df38, 0x1399fbac37cef9, 0x7cde86e8534405, 0x52e942177f08c0, 0x3a56e5cd882aa5, 0x78986a044f6da1, 0x14aef390f02fb0 },
        },
        {
            .X = { 0xebbf67a3c090ba, 0xa4b0fd639b0025, 0x5752e95c8ec44f, 0xd6466e61c8f998, 0x95eec27ec7c121, 0xbcf1f78c8566ce, 0x5a63c8ba7f25ed, 0xe9607551d9f67f },
            .Y = { 0x75a9446b19c01b, 0xbcba8a576b599, 0x391684dd2fe50c, 0x4adaa96f05177a, 0xe0354b255d134a, 0x6fa00812617127, 0xf0544b47f38e89, 0xc60f7e44e31c7c },
        },
        {
            .X = { 0x645a7952b3ee91, 0xa6d30603ee55ba, 0x9097f3fc9e99d8, 0x48c9c5a8e46c4d, 0xf5d60522ed3ce0, 0xdacb45d1d44409, 0x36ae72a649f3e4, 0xb359891d722658 },
            .Y = { 0xf0d8eb0168ff23, 0xb5663672950acf, 0xd1464978de047e, 0xe369081de9176d, 0xd583d49c2f8ddc, 0x9a222b0f2f376a, 0x2213813fc656be, 0xdf2df5aafa7d21 },
        },
        {
            .X = { 0x233a113ca65768, 0x6a353c6b01f9e, 0xa49c78df53b03, 0x28f15cf44b82ac, 0xd1739c6d71e354, 0x2e632120d9f81f, 0xeabb0dc7e38921, 0xc37112e503b28b },
            .Y = { 0xb2d39d645a8a83, 0x17e807f8537db1, 0x8ec0c6fda3f05c, 0x501d4475c794dc, 0xb2eef94c3ad7f4, 0x8b758fc1499a6d, 0x8ca0cb69b0e5dd, 0x9b2aba24de5661 },
        },
        {
            .X = { 0xc17aa577d8f829, 0x5b700145d3107a, 0x32ae428d50e827, 0xe1f62a14f0c52b, 0x62f42b523e3ab9, 0x75890eb669a183, 0xbc88f83f2b957a, 0x888ffae9e637a0 },
            .Y = { 0xb0b35b8128863a, 0xd6eb0754e13a48, 0xd2c64bb07d9801, 0xff5358c36bd537, 0x8aeb05671ecdc2, 0x907bf3ea731e5a, 0xed90f7995a0bab, 0x9660e5512bfbde },
        },
        {
            .X = { 0x5b65df15789357, 0x62481988dbe9aa, 0x733749b34b7d99, 0x27884b9be3618b, 0x2253d321ccf6a2, 0xd92b27f4df9083, 0x2d932fcf113753, 0xa60c889f3ded6 },
            .Y = { 0x25a220410e0ee1, 0xda529e424eb2e9, 0x84f307a84d8a30, 0x1542308b1c5263, 0x28665ff2eeaceb, 0x120b209fa14b4d, 0xcf4f106b50931d, 0xe5503bedba1a55 },
        },
    },
    {
        {
            .X = { 0x21e36765a340e4, 0x65964cbfc72d16, 0x2da61f4358b7c1, 0x7e130fdf4df9cd, 0xd8b35858044c79, 0x6df94832ae26c4, 0xdbc49236a14f2d, 0x9b250fafe388d6 },
            .Y = { 0x192c7c94319eee, 0x7d5a5b77690623, 0x60ece3946757b, 0x58ba2e8070a49, 0x3a38f9c92ead81, 0x3b776850ce8776, 0x138d5aef13140, 0xac7b04ee1b4506 },
        },
        {
            .X = { 0x2e66a8cd4824f8, 0xaccf71b1225e6f, 0xfc323b3de086d8, 0x9757294e26188b, 0x1e5831b9ac333b, 0xf18cf0ce88c85f, 0x31294e6d399ff9, 0xc7655dac110744 },
            .Y = { 0x17a8eb36f3a824, 0x36148894c57fcd, 0xf08ba30c82bbbd, 0x5c4a4d49a992, 0x199933f771662a, 0x4183af30197364, 0x2963e853b53d2a, 0x457258ce442172 },
        },
        {
            .X = { 0x9320a66da72264, 0x23a0384ea97cf5, 0x701504dda7667b, 0xd2718e19b387fb, 0xf47a9b4b14ded1, 0x1e163c0f9953fe, 0xdafafc2bc8b3d9, 0xba0e231048f9bd },
            .Y = { 0xa690502593de3a, 0x21ea23eeeb2cea, 0x254ebb1b67a091, 0xba42d12a34dbd6, 0xfd92bbecdcd365, 0xa3db157bce9136, 0x57fd0cf70384ac, 0xb8feafd94c260 },
        },
        {
            .X = { 0xab0917ab71ee5e, 0x6958a0c6b5004, 0x34ec215aa1e915, 0x3e06a262c9139a, 0x2d0ac0b75773c, 0x220d57a7ad8f36, 0x876f79f99fdedd, 0x5b8ba2702dd7c5 },
            .Y = { 0xb43c8bcc8cf47d, 0xd080d6f8378756, 0x8d46e2bee99cb1, 0xd028f043fde414, 0x67a85376e350df, 0xa3c11f2b4371ce, 0x8d39412b45ba30, 0x5b65081c7f2823 },
        },
        {
            .X = { 0x66f07fab9acbef, 0xe2fb621bb7a775, 0x7b49a6e4a25d3b, 0x3eb791e9d3333e, 0xc1cf93a09cef3, 0x3580d552d50d57, 0x863f4fc1b7d179, 0xe0e295ebb7e16a },
            .Y = { 0xb03e19e05eebee, 0x4191a82a13cb2a, 0x69c2bac6fd1596, 0x1e34eacb64c4f1, 0xbeda32dfd11c74, 0x9af0ba50391469, 0xb63dbcbf7af978, 0xf3f630beba22f9 },
        },
        {
            .X = { 0xecd7f29d72278e, 0xea102865e32a69, 0xc64d1cecdf3f47, 0x7b9bac29e46820, 0x7708467444e543, 0x35c68adf7342ef, 0xbd77d19f2aece1, 0x53dc9110963390 },
            .Y = { 0x4a8eb011afad53, 0x77831b94d01, 0x9b5ca084ecf6a2, 0xc754c1ede6d35, 0x4293efb3209714, 0x1cad8f295c3771, 0x98f6eb4445ee94, 0xce51c39fa3f8bf },
        },
        {
            .X = { 0x393f116395ae90, 0x594f2ff349f3bb, 0x3a676c5948c7c2, 0xc9d5eb52830f52, 0x75d09236a99931, 0x68af6e2e3e2147, 0x473c18b85f492c, 0x4f3e0d509060ce },
            .Y = { 0x4ff1fa6375f0da, 0xbfde198928db08, 0xdec4082efc1f7d, 0x3479a90e085cc6, 0x733ffb7e8adfe, 0x994a28a804683, 0xf582fe1f93b64f, 0x903159754e4aae },
        },
        {
            .X = { 0x89ff26e8c1a551, 0xe53c64cce1949b, 0x873cda3ed6a696, 0xdb4aff7c28cdab, 0xace412c5a013d3, 0x245400a49d8918, 0xad2153a497dc6e, 0x8bf3c7cbb55fa3 },
            .Y = { 0xbb6217c7109422, 0x8a170eebffd3fd, 0x332666cac8e924, 0xc0db1a3bb8825d, 0x8a406d3f698aa, 0x7e3e25cababbf0, 0xd505b1ed10bc35, 0x6af24be1764b33 },
        },
    },
    {
        {
            .X = { 0xea22795f149557, 0x7be3e663978883, 0xc931e698fdcee, 0xaffe3feb885126, 0x904f74b55a377b, 0x54c77a0ca4cda0, 0x98ce161fc0eb86, 0xcd0fc818efa01c },
            .Y = { 0xc6f0021d63f4a, 0xa0dd08d4b8b3d, 0x48e021e35e5fe4, 0x7afb2124896323, 0x152e8c01854884, 0x8deee7e3ff35a5, 0x6158ed5dbd29ae, 0x4907b017061654 },
        },
        {
            .X = { 0xb04587744517ad, 0xe7c3ef97c2cecb, 0x2d02d11530141a, 0x2c9368fac482a1, 0xb5d1ebf606818c, 0x9fb22b0e69c1ea, 0x706b41b4ad18b0, 0xf155174d18263d },
            .Y = { 0xffdfb1b458f8d6, 0x6e8c9f7594441d, 0xb850f667619685, 0xcb18d1ca5244e5, 0x48febf8f0f8544, 0x88fdebd7800940, 0xebe0a993a5ba39, 0x6cc203106a6b05 },
        },
        {
            .X = { 0xb79d47d5bc3861, 0x22f17c863c8949, 0xc7888129db5625, 0x6b2e492225d04b, 0x7c6e3054dbbbd0, 0xe3186f2449016b, 0xa00bed34a6b691, 0xd7e6335bafc05c },
            .Y = { 0x345164bb55ebc4, 0xf523b4521c694a, 0x44f946809dbf39, 0xac8f72fe0798eb, 0x1f94f02dd94ac8, 0x55032f68016bbd, 0xba06d49267ead9, 0xb56886181879f0 },
        },
        {
            .X = { 0x1dcf6261dfcace, 0x282d2b243c458e, 0xe9df7f42461b93, 0x1f0b6993485a32, 0xa16844acf5797, 0xe8fd7a65b15b72, 0x7db3576c61a40e, 0x8cf9d292a5a69e },
            .Y = { 0x4805c9d7884492, 0x94639a59ce72d2, 0x26ef0b0a9a2a42, 0xa82a75615ff9c2, 0x2ed7c000585953, 0xb66525591c1528, 0xdc49f770e86b51, 0x1a5badb21140ed },
        },
        {
            .X = { 0x644e920744b177, 0x8bac4b2a8c73e6, 0xca8c9619a704f1, 0x579f366d9d2329, 0xe298f0cfe614eb, 0xdd48452ed4cc40, 0x2542e18544d065, 0x1980641bd84a9b },
            .Y = { 0x5c1d9db0e6c575, 0x37090254d50bc7, 0x6affafe5b04797, 0xd26d23c780cd42, 0x6640b5084f5c26, 0xdae9a85f0a881d, 0x7ce755b08b442e, 0x9c9fb762857d7b },
        },
        {
            .X = { 0xe8c2b1b29984db, 0x54264d9db7f811, 0x22c90f5e3481e3, 0xab993cf997cb95, 0x7ef1cbd1da89fa, 0xa0d19ea1abc961, 0x47a82c8f1a53b9, 0x92e4130d8fe9ec },
            .Y = { 0x1bd8e3f04e84ee, 0xbba09bf4276e5e, 0xf21cb693f8040f, 0x9ca4c36b7c7859, 0xac9fbe22dcf385, 0xa7dae329cc4d4c, 0xd401bc04595088, 0xa11da9e27b50bd },
        },
        {
            .X = { 0x6c495b922e6feb, 0xf5943be54c7351, 0xe44f3e1cda18ab, 0x106859365b8f0d, 0xb30fa7ed82d71d, 0xadff7e1fa4fee9, 0x443f5eb01b056e, 0xfcfa2019910811 },
            .Y = { 0x56e6936701d5cb, 0xab834345fce2a, 0x6712b3179d5dbf, 0x19b4d444dbc57e, 0xb955d0d92f862c, 0xcfd735e3d024cc, 0x99fe7ce4dd860a, 0xec618b71a5cbae },
        },
        {
            .X = { 0xdc720c512f3b93, 0x4afe3a128cc72e, 0x1112e4f854872b, 0x6d7701fb99e08f, 0x53a6c364777fcb, 0xeea789d06d8fbd, 0x1209bd33316778, 0xff1beaf08e25a7 },
            .Y = { 0x393dc3e514f345, 0x59cb2af79fa2c8, 0x86dc3ca64ede0, 0x61e8492491ac39, 0x300bf03bc07e86, 0x526340b97e06bd, 0x629ec1752098ef, 0x60e92379182c5a },
        },
    },
    {
        {
            .X = { 0x214846c3e58f97, 0x948b70cc94edf1, 0xb5849fbf5fe009, 0xa5bf73583f4d56, 0x94dd133f530741, 0x17ebcfcb5c6472, 0xcbcd3d8e1171b5, 0xfe2aebb7cd528 },
            .Y = { 0x15a39020519f7e, 0xec531e376a3158, 0x4f605cfeed89a5, 0x5422f1d5346cd7, 0x234e27f4951ded, 0x98c37979578bb2, 0xfdb6443d459c76, 0xa69561f101ce6b },
        },
        {
            .X = { 0x85935d305480e3, 0xbaa3be1713ed1f, 0x8d14c0df8e0285, 0xc9edf6e113625e, 0x546f8a86f52fe2, 0xc9c51bcb63c227, 0xfb3e7dabe4861b, 0xb93baf6795e4ee },
            .Y = { 0x4b10a5a313e81d, 0xed0f6dd76395d6, 0x372d7ff5092d72, 0x85cf853722f202, 0xa73aec15d9ecb1, 0x94abd2fc98e251, 0x97332341c9c731, 0xca53934a26bc16 },
        },
        {
            .X = { 0x1569ef85bd0c47, 0x7f9ad6a5b731f4, 0x5dc52724c595d5, 0x378195d254d2c6, 0xfade632c39e6a8, 0x29fd05cec08c51, 0x2ee07be1196dd0, 0xb17b30af1e513e },
            .Y = { 0xe622fbf9b2c91a, 0xe14b7fbc7b3e4c, 0xcf772984bf3ba6, 0xb1e484eaad58ac, 0x1ee9db3846f514, 0x670b38a8e1e1ab, 0x36d64f5d6a0b32, 0x3228ca191a51f },
        },
        {
            .X = { 0x54c6c548928855, 0x548e358d8f2b6d, 0xf770d79c487bbb, 0xc27a6bd1a2f7a1, 0x1d92d436bf7b80, 0x59e3869eb02080, 0xe53aee4ba4313b, 0xc1178543143942 },
            .Y = { 0x83cbb3abffe3eb, 0x354015e36a5f68, 0x6e0e7119f7cc09, 0x82f0990287d7f, 0xb5d85eb79391a2, 0x51f9c6c228b7df, 0x84d75869bc2603, 0x3355bc5723e006 },
        },
        {
            .X = { 0x9ce2fa912bc745, 0x52fab703fbbf9e, 0x6ceeb3dec6c858, 0xeea0dd7e6ff761, 0x2be4756d9b6921, 0xfcbedc98809146, 0xed9e72b634097c, 0x40a4fabd1bfbf8 },
            .Y = { 0x9b16dba7f0b280, 0x47c1058151b6b2, 0x498b367b994cd4, 0x39669af936a386, 0xf04f64acf1d6ff, 0xfdfb8ab972bd3e, 0xf46ab3c4a2e807, 0x21481a71f73438 },
        },
        {
            .X = { 0xb1de95f2dbe996, 0x4665f06b536023, 0x4bca9a11a6e8bd, 0xdc460b720e3a2e, 0xe042d1d8f6229d, 0xa8e6fa92cf93d6, 0x2c0c8ea1bb71be, 0x36eea1d35db55c },
            .Y = { 0xc8a70450e02374, 0x1280ca95fda59a, 0x33f051c961e528, 0x59dc3fa53d6ce3, 0x46e46bbbfee1c2, 0xab94a5108130a6, 0x3bdf6ffa7bb61f, 0x4e6b6ac7886d6f },
        },
        {
            .X = { 0x90a2c0850a167f, 0xfcc6b4cdbf81f1, 0xaa2786f837ddd3, 0x5a6ceec998b762, 0x5ff44067c861d7, 0x7a885067f3ea82, 0x16862911de27e2, 0x9903b61485bb00 },
            .Y = { 0x82ea80a40dc2b4, 0xd44f8183b382df, 0xa20abb7166d55, 0xfa9a418cfbafb, 0xf8e64443010159, 0xc71df9724a8bc4, 0x35d0b0ff8161be, 0xdffe1dacb04d6b },
        },
        {
            .X = { 0xc6ce04f17063ae, 0xb6440360c686be, 0xba60ea17c2e7de, 0x27c08b70761e4f, 0x83e9a8f34ac96c, 0xa693fe2990708e, 0x7b88d7861fbf63, 0x9f26b4058b210d },
            .Y = { 0x3afd380a818de8, 0x757dccea3b068d, 0xc876e827ec352b, 0x3161a944a0f7cd, 0x3450c8fc88f8a7, 0xd01ea8e87594d6, 0x9a57743f41e07d, 0x3994c54b2adf07 },
        },
    },
    {
        {
            .X = { 0x9ac35a3b5d0744, 0x55d14dd493ad18, 0xcafffc1d5e5312, 0x39ebefb02cd15e, 0xc250c234d61a60, 0xed5fd9ad5339, 0x8d0f8cb5a070d, 0xa99849152dfa94 },
            .Y = { 0x413934f3422202, 0xe87ddc146c0cff, 0x5be4c2e66fb6da, 0xa0cd6ea1ece0fb, 0x9f6482e2f5a9fb, 0xfabe7759fd5338, 0xd382918e7a368f, 0x5568047fc8e655 },
        },
        {
            .X = { 0xb5bd0afc3a3ad8, 0x2134dbdf35aec3, 0x4f50c02da27ad1, 0x112d079385e629, 0x96bd9f72d9a9a, 0x37e4e5b41cdc5a, 0xe1b08a1f830f06, 0xeeafc15926e42d },
            .Y = { 0x9da90cfa345547, 0xdbff3a0f70115d, 0xe052b23492574d, 0x6673ca931fdeb9, 0x1d6edbf03e71b4, 0xf3961e55256bdd, 0xdcdff6a14fce04, 0xef807f5a94013f },
        },
        {
            .X = { 0xacb6d1fb54d20a, 0x8fdf91a8079875, 0xb45f33db97b3aa, 0xe4e1ebfea972f8, 0x6c284e229abc75, 0x78eb4b22742c1c, 0x64a7f3abf2d001, 0xa50c3fc9c90830 },
            .Y = { 0xb15c2ef67eb6aa, 0x8b99994c23ebe8, 0xe6129716e7a91b, 0x38acc7208d4610, 0x775a73a4e5356b, 0xba35150108d35b, 0x8b8ff303b8ce98, 0x85cadb51656165 },
        },
        {
            .X = { 0x55682a2faacd41, 0x9743089b06cc22, 0x3ea5264fc586bf, 0xb2e37adc62f5ed, 0x3e188c8cbfeae3, 0xf8affa2058e10d, 0x46c48423a6d393, 0x6517ff488d716b },
            .Y = { 0xcc466526714ce4, 0x900d0f1a1e4479, 0x887a6f16504570, 0x798ad96e543257, 0x15a81495cf66a9, 0x401b34991182e8, 0x9599065e4ccf2b, 0xe09dae9c01982b },
        },
        {
            .X = { 0x95dfc721768b14, 0x439e6273668161, 0x139696f664a07a, 0x23be83ec31b414, 0x7b992250161ba4, 0x7ed1ce898eadd4, 0x28cc473a9730c8, 0x2145b703cf5f38 },
            .Y = { 0x212ad8fb10c887, 0x8a150229055f72, 0x767949fe49a789, 0xcd86384b3aebc8, 0x453b4f041d1bda, 0x5fc38132b8c19, 0x4ae1b9ba4ec1f6, 0xfe099d93d766a1 },
        },
        {
            .X = { 0x6f5db38f3f4bdd, 0x10d1c5d59d5045, 0xfd49948c4de99c, 0x8408010078f6a1, 0x715b45a486a041, 0x943ad1b87c6fc2, 0x571de1ae376113, 0x874da35c677bec },
            .Y = { 0x785ea96ad085be, 0xd7fa592c735c71, 0xbaf4a129369193, 0xb9da283a627c85, 0x233feec877c1cc, 0xa0c8ddfe63f176, 0x1dab4c5ea8a6c1, 0x4772578abe7833 },
        },
        {
            .X = { 0xd80c0815763a01, 0xc3f629a8ee189a, 0x8925ad1f1d6b2b, 0x41bfc34d708ff7, 0xc92b0a35234c66, 0x8ee62cde61ea57, 0xb611c6830b626f, 0x5360cc059a7072 },
            .Y = { 0x38480ce3690a6b, 0x2e386f61836088, 0x8d60a4d155b792, 0xbedd4144e3507, 0x9c41fe25a8e8f0, 0x184ddbcaecf591, 0xd11a7b3a239c61, 0xf42dd5e166116e },
        },
        {
            .X = { 0x1bb8eb8a909f47, 0x2b9f8970d5ba13, 0x49f2bb0bf3c8e1, 0x5f18f9ea980e4d, 0x3e5e0bf17803c7, 0x9e9c029254be3a, 0x8f5acd1debd8a, 0xa4c8617d65c0e9 },
            .Y = { 0x20fc6e85fc9f3c, 0xc5fc4bcd775136, 0x3d82dd18b1985c, 0xba5064613acd96, 0xadace8c3d6aad4, 0xd16ac29498b0d7, 0x8e7fa7b0402af9, 0x3a2166846de3a9 },
        },
    },
    {
        {
            .X = { 0x95ce121ffb5047, 0x7ce28bee24275d, 0x1c8082b996f651, 0xafb5eaf4bb3050, 0x3981caabe876d2, 0x3d67790716994d, 0xe97fa6458d29b7, 0x7913a9d85f62b8 },
            .Y = { 0xe9562b090e490e, 0x1e38e5eb7da5d, 0x39ee32e75d3b89, 0x903a3a140ac9ff, 0x1874757c2fa8e0, 0xc305a117d2e842, 0xf53792255acff4, 0x88d6d61288fc68 },
        },
        {
            .X = { 0x61276b46b9870f, 0x60280a2c6b20a9, 0xeb6c06faec0ec6, 0x359f9203e12913, 0x294ca133d54359, 0x54b05e79d15e3c, 0x58dee3a2d4ab00, 0x3ac8ec3694c0c2 },
            .Y = { 0x8ca73c6c62d6db, 0x8a0110b233ef95, 0xaa9ae4dd6b603, 0xfd56d1924876ca, 0xa7623d0d5330ae, 0xb635278322a9e9, 0x75868dcf4a4793, 0x8f7758221ec23a },
        },
        {
            .X = { 0x36ac6f458893d1, 0x645c43946fdf66, 0x8b20ff303328b8, 0xce567afeecb043, 0xe25e09744ec6b9, 0x6950f361b406d4, 0xcedfc32c85d54b, 0x4177d00d9ace87 },
            .Y = { 0xe4f5d3bd87fff4, 0xaeb273d73b087a, 0xc9823c501e019f, 0xe929d70aa69e67, 0xc93de03197fb54, 0xd9e98b9173256a, 0x318298479d901c, 0xed635aaeee66d3 },
        },
        {
            .X = { 0x252d3161db22d3, 0xfb8b6bf42455c5, 0xfe96baa5c5bf9f, 0x6b9789c387bb78, 0x3febd2e1658040, 0x854ffcf0518dce, 0x5d0a28aca79b67, 0xb54045ad10f901 },
            .Y = { 0xa4f160c22db24a, 0xe8f4a29261f915, 0xcce207267c8289, 0x8c110560c31f4b, 0x2400294d2253d2, 0xeeeebf238ff63c, 0x731aba42f17533, 0x925d439454f87d },
        },
        {
            .X = { 0x5e35cab2512c5d, 0xa31b4f6ca32981, 0x4086138bb5f10d, 0xbcfcf4d2c1b40d, 0x4526641ed7d8e8, 0x6cb1313d1fb785, 0x2781be80c1ca07, 0xbb346584c27c88 },
            .Y = { 0x1c0ba78df8cd80, 0xb82d5ddef2c759, 0xa1fc74cac62164, 0xcc3d75e4f5f66c, 0xd92989f0fbb587, 0x80b2e99439bde8, 0x73b85917018e11, 0x3acdd725c18060 },
        },
        {
            .X = { 0xc4b09bb01fa05f, 0xf9703fffdaeb4e, 0x90cf67381a36d4, 0x84ab8f9b6aebf0, 0x6d0fc8ef847a43, 0xce3446581255a0, 0x6f20ca14887b18, 0xd23fb203e37934 },
            .Y = { 0x38eb76a2373565, 0xb406e861d526b6, 0x12c6f62132ece6, 0x4eadda5662b727, 0xc060361b677471, 0x74e529e144883a, 0xb8d6897a92076e, 0x6249ad3e7a26c4 },
        },
        {
            .X = { 0xc5266f6e532824, 0x6c5ac82491dd89, 0xc1a5861c9c49fd, 0xa67d3cb5ee7c9b, 0xe64b2fc362650e, 0x38901943800b9f, 0xda3a7cf0e81d6f, 0x76c9508b372b1f },
            .Y = { 0xad7f48a3328f73, 0xd4d95afd9ae5be, 0x7e3c9984e701cd, 0x769e873d9cb2ba, 0x4bbacba9925abd, 0x83632a11f1a507, 0xec985822a00f70, 0xe11aded7451cb2 },
        },
        {
            .X = { 0x80390b3f784ede, 0x9b82184dfa05e9, 0xec0cb0fac26d23, 0x3b210a918febee, 0xa500ae2e461e15, 0x216ec28a20e32a, 0xb2d4a4049adf77, 0xe2feac54a52bef },
            .Y = { 0xc56ee29b231178, 0xbc9c7b291da66, 0x3b419252aa174b, 0xe8d67ef4a7299f, 0xc0c10a17acbb26, 0xc31479f5c6b940, 0x7d67457b9dd1b2, 0x4424d81f60ec69 },
        },
    },
    {
        {
            .X = { 0x9fcf50001ca366, 0xce5f4bc94a8dee, 0x42e764d2bfa9, 0x6ee5e73ee152fb, 0x149ad05bbfccac, 0xd09222adba6380, 0x745dcebcc90676, 0x3c86dc5831ce70 },
            .Y = { 0xff18ae3a6df6e2, 0xa149a42b85facc, 0x5d2858b7462860, 0x3291830f75594b, 0x267426ac44f9ca, 0x313abc04c476e4, 0x4bfa6c113a271f, 0xfd208834da52f6 },
        },
        {
            .X = { 0x2ed4531ca98fae, 0xc72dca608765, 0x729049168e9a8a, 0xabd28d029e6654, 0x794907085abebc, 0x487afc0f43d352, 0xeac5746805a703, 0x492d233badf0e4 },
            .Y = { 0x8475b8cadf32f6, 0x16d447df741e7, 0x79edce0fe14f6f, 0x651409c79660bb, 0xbe4eda88055268, 0x25f75ec8587f7e, 0x907ef4f9dc0bd1, 0xb0312073088e77 },
        },
        {
            .X = { 0x4d8f538901cd50, 0xdc8ea54d943e7a, 0xe38036489fd8d0, 0x99c0aee4943eaa, 0x44675978b27f9d, 0xb234c6ba6da3f3, 0xd2572cb91a0808, 0xb724425ede4da8 },
            .Y = { 0xe3f689a7192548, 0xb5b8aa27e8744a, 0xf8de9e6192747a, 0x1b706f470720d, 0xcad6aeee002306, 0x441b3326f8e241, 0x1785691f24d92f, 0xa88f0e0438b315 },
        },
        {
            .X = { 0x2831d0e7377307, 0xbb99615f6c0079, 0xd0d6f895225f40, 0x99f069f9817303, 0xcf09af75f9f18a, 0xdfa4b14109fac8, 0x8d517ab284814b, 0x504189b953aa03 },
            .Y = { 0x6e46ec411a7277, 0xacd73ae2ec2e54, 0x658c5281c9902d, 0x41dfdee6800dd4, 0xbe2fb40d579e2c, 0x737935db408bca, 0x5df49b11043915, 0x9ee79a826182fb },
        },
        {
            .X = { 0xdf7640828f17e8, 0x702b97dd2585f6, 0x7e590d4f7f6bbb, 0x3c8f317c9e4861, 0xfe875530ab960c, 0x5cdf638b92d9c6, 0xdc413beb533109, 0xee5bf5ad330f75 },
            .Y = { 0x2b74adb6ad34ee, 0xfbe505e384fd4a, 0x2b831abfccad14, 0x24703db012e8f6, 0x9e0aa5b14ed359, 0x870c0b16dd558a, 0xb3929829f5d61f, 0x47740ed7035d83 },
        },
        {
            .X = { 0x18c70d82239dc6, 0x3d4b7e7c5caf53, 0x8b85eefa45fda5, 0x7eda75a780fea3, 0xbd2c46d0574112, 0x69a8a02af94ee5, 0x955f825996be1a, 0x544141352316d8 },
            .Y = { 0xa22dc6408bd77f, 0x80cabb981c9bca, 0xba85acc47a3b47, 0x9a8b949efe46b5, 0x3cbe9955bbe0ac, 0xa31ccf67b99eec, 0x9323378086f700, 0xe75a96538bf85 },
        },
        {
            .X = { 0x5bc50eb1b3bc43, 0xb1dd8c242ce7e4, 0xed10810c922209, 0x58e028a73797d8, 0xf7643482bc3912, 0x64df000663e8b, 0xf5417e517f770b, 0x59b5f5bfaa6fbe },
            .Y = { 0xaa5e23b5e23683, 0xaa7956afaaa90f, 0x97c76b4c799d45, 0xd98960009f7665, 0x13cf9f05eb205b, 0x9f795a2fa265d9, 0xc55873f802bc0e, 0xeda6cffbd2719b },
        },
        {
            .X = { 0xbdce78d151c6ad, 0x93d1597ea5e59a, 0x570af29cbbb477, 0x2a5cf35f6f800e, 0x12870f68a0bd7c, 0x3b443335c4d8ae, 0xce92a5a98dff5e, 0xf5a4722add5a84 },
            .Y = { 0x5d846f277195bd, 0x7de45b07a1e88a, 0x94cd9194f0cf1c, 0x699308231b7281, 0x3394b8b707d92f, 0x98ce18ead8572e, 0x576e3dd73ef55f, 0x24605beb9c1a60 },
        },
    },
    {
        {
            .X = { 0x4e4999845285e2, 0xb336e3a06f6e8e, 0x3552c792d4a1e8, 0xd3f833dfb0e76c, 0x79060eec2ee5d7, 0xb54c92830458c7, 0xcfdcc8b0bad051, 0x698571bde4cdab },
            .Y = { 0xee422a34267767, 0xd59d210d2a7d9, 0x45a794a1c905e4, 0x9b209cab6c6ed2, 0xcccd457eccd4f, 0x72603cfc1fcb91, 0xf81295fbbf8ea3, 0xd18c7c83df691a },
        },
        {
            .X = { 0xfd5ff126ea8f8c, 0x20f3c2573f6fae, 0x832af209b89fa5, 0xad14f6f332b237, 0xb6969449666e81, 0xa6bf98c811e800, 0xb6bad3f63e8962, 0xd0428a9f2f8e3c },
            .Y = { 0xa0c41015068bdf, 0x75a1ba6f8bb9c7, 0x286bd7649fb838, 0x942a211c1a6c31, 0x5e5e95b5845bea, 0xd2de684b687c15, 0x6059ce591268d3, 0x8ebf81c45e374f },
        },
        {
            .X = { 0xd71dd66ff2dff7, 0x22048787a2c97c, 0x2a5bdf8948e6c5, 0xb12097a92970f8, 0x5bf85e7c8c659a, 0x7f18878821cf97, 0x5136e5618a4e3d, 0x65eb68c4ce50b7 },
            .Y = { 0xedb4f3fc8383f7, 0x5290310af7fe16, 0xf137f88823ccbe, 0xed5489f6e16c6d, 0xb9c6eec8c20015, 0x8f9b68c8f95774, 0xcf9c97d0a31be7, 0x57d64492fc065a },
        },
        {
            .X = { 0xda2a7f29771cc4, 0xaeb07875083dfb, 0xb6382e082f901, 0x1bc9565318bd6a, 0xa2befb75df6d3a, 0xff3d6d63cbee37, 0xe32294d0f9a97b, 0x9eef9d05968a28 },
            .Y = { 0x7d38729edd448e, 0x9775c67cc8ed89, 0xfbfc17622a55db, 0x264aff1afd359c, 0x3e1580e1fa22, 0x90639c125b5089, 0x451ca31b8d9d5, 0x58e91de5b1c079 },
        },
        {
            .X = { 0xf486ed269aeba2, 0x53812529ccd046, 0xa8edc6913c9b64, 0xc04d1669b8a852, 0x1b3716298d944a, 0xb8247064237291, 0xbace734e74988a, 0x4ee329f4b02e10 },
            .Y = { 0x8fac422e5dbf7a, 0x2ffc41847b3372, 0x3336f21bfa386d, 0xe39625f143d1d5, 0xac1674d1fcb951, 0x94628115472bf6, 0xb23a5f2f033a0e, 0x9b6ed3c638c6cb },
        },
        {
            .X = { 0x1eefa3584662e4, 0x1a1b4431c04ac6, 0x8d2a4be6f5efc6, 0xc10060c999aa3d, 0x72567c84c2f925, 0xe4fad543b02cfe, 0xd1d8fd3a800117, 0xc384d12f03cf71 },
            .Y = { 0x5327f4077b096a, 0x9b5835a760e9ab, 0x10581e9b105ff, 0xf15c151899406c, 0xd859e8ebc19166, 0xbfb8f4e8c3c169, 0xe4ec579ef6bca1, 0x66932a8f6431b1 },
        },
        {
            .X = { 0x5fa0b968f49c6e, 0xcb1514f9f8c791, 0xc77ecf98b53ce, 0x25f60da4b77f73, 0x17e43628c056c8, 0xae6a00056d86d4, 0xdd0889ed9f44be, 0x42e1faeef5b869 },
            .Y = { 0x23affc823fa7e6, 0xbb0d38b3039692, 0x940212f5cd9c17, 0x93209910f68738, 0x9577d159117985, 0x51c7a9d5b9d264, 0xe702ecbec937b9, 0xbb862f823e1371 },
        },
        {
            .X = { 0x1c1ad461bca7c, 0xbc63bb56f7bd6b, 0xcd5aa0f49b325c, 0xfa3390c5998fe2, 0xebe5ac21609b81, 0xc99094ecd518d6, 0x33df2b38b64ccd, 0xcbff0b5b09ca4c },
            .Y = { 0xb339cc48cda0c3, 0xd68989861eaf2d, 0x767a294d8896ef, 0x59f26a0fae6f47, 0x515c96bbaa8d90, 0xbfdd44a46849e, 0xdac99238795b64, 0x982c11eb015a7a },
        },
    },
    {
        {
            .X = { 0xa13c0d50c8eb9b, 0xe46c53a870d3b3, 0xbbbd5be9ed2c89, 0x9a00ff594c51ca, 0x787afdde54a901, 0xbbd08f34d3acdb, 0x334b6d03fdd58, 0x37b760ec037648 },
            .Y = { 0xb52e2130c96e94, 0x455483e9a91430, 0x32336033b846d2, 0xf7378a93275253, 0xbffcbcd224b884, 0xa2ffe85598a46a, 0x5e7275efcfab98, 0xc796065601dc00 },
        },
        {
            .X = { 0x80eb80b02bcb3d, 0x4fa07ce4bafe72, 0xda6fb458825cb0, 0x828499acd67af7, 0x65c29922fb73dd, 0xf42b0e55a9da68, 0x37d7df614dee16, 0x99d4ae24d38ff1 },
            .Y = { 0xe9a2a26fea77ed, 0x1fe37d9295d9e3, 0x9e6d31b70e3f4, 0xbeba00c3cb21bd, 0x6549d6e94c8967, 0x1fb7d25b4cf765, 0x3d573544df4440, 0x35fca118ae85b4 },
        },
        {
            .X = { 0x9406c7400f7afc, 0xb11fa5c9ecd95f, 0xb2ec185fd2f85d, 0xc93bb5df860c2c, 0x36c80625719a15, 0x49ac613c2b7e89, 0xdcc3c4d4b33c46, 0x409f67a576f06b },
            .Y = { 0xa8508312f7e367, 0x935f52ce2be6d9, 0x3bfb4f6c8ac2d4, 0xdfd390bd1afc0e, 0x1019f65949cf1c, 0x394afd18b26ee4, 0xa59bbe19d6f57, 0xc6ac4087d75dea },
        },
        {
            .X = { 0xb9fbc7827d19b0, 0x7b324de4450c52, 0x847cdbb3412141, 0xf043677f5e8fde, 0xb5f694642e0048, 0x9b3e67887c6cfe, 0xf89eb36ef16e38, 0x9db2dc6cca639d },
            .Y = { 0xc67d121c2dbdab, 0x9624146d538f05, 0x894a575c866b61, 0xc7c3289efbf982, 0xca92282c2e25b9, 0x4f602fe532c95b, 0x9ea562d1c09096, 0xda760d791fcb53 },
        },
        {
            .X = { 0xcfdbdde5de1471, 0x2229e278b9328c, 0x2407b106870d8c, 0x9768a296b503ec, 0x10ef92452856cd, 0x6ef62cb8ab4f03, 0xdb83bf465f7370, 0x734777db453968 },
            .Y = { 0xb03f7b18efdde4, 0x7d2f9a214f9e1c, 0xc584e8a289efa8, 0x39b37a66e10475, 0xeb4d866bab7453, 0xf93a173c189ebc, 0xce01c379764701, 0xbcbc0f2ab4317 },
        },
        {
            .X = { 0xa8cf0e79ee12f6, 0x1c16891ba4d270, 0x2162f0847ca94b, 0x2735faba72ed40, 0x45076d4fc6e3d, 0x1755e3c671a2a5, 0x57fb369100c359, 0xca3757e0e27d80 },
            .Y = { 0x12c39efabb5191, 0x747441be43020a, 0xd6b22fe77fd9b7, 0xcb80c3bbdbb9ac, 0xfec89f2a5533a3, 0x93fc9bd6d7ea4e, 0x19735b8d676da3, 0x644c55c3760a28 },
        },
        {
            .X = { 0x47bc9edb9ecd5c, 0x92624233378dee, 0x3721a8ee85fe82, 0xfeaf0ff9e9e0fa, 0xc83c973e229396, 0x2e1251dde342fb, 0x4df5cede92d7d7, 0xb566d4bbfa8b84 },
            .Y = { 0xf52cad827e6eea, 0x3f3eb4302037bf, 0x408c2ff1e46ade, 0xc581eb7a127ecc, 0x88e876e1924e38, 0xf9e14456afaa29, 0x93a2ccf4fee4df, 0x965ded23ec0c11 },
        },
        {
            .X = { 0x65b42a9cd6ce5c, 0xb0c9a72348e1e1, 0xd3d0ae250d3659, 0x920c4a6d4581de, 0xde829a3390fcb0, 0x812d080c74d44e, 0x8eec799ba01481, 0xd48d32a3077cf5 },
            .Y = { 0x78e9e3f19d276a, 0x2a77b622370da7, 0xa84f6e258258b6, 0xa3a148379f100c, 0x2177c22a3026a3, 0x6e04c47b56b5ea, 0x7b2568fc6fca15, 0xa18edf9592191b },
        },
    },
    {
        {
            .X = { 0xe3a9c576104511, 0x3ceda8756d0369, 0x259f659bbfb78f, 0xe961b0fe109b47, 0x6da8ed33d94c07, 0x5208a0dd1400e3, 0x704a751dc2ce25, 0xa83e41634290ab },
            .Y = { 0xf6e41e02379d6b, 0x6f51d700498f50, 0x35e67ce329d52c, 0xf163242a975766, 0xc779ca7980cbc, 0x343a2230f0434a, 0xfa7c80eb6b9dd0, 0x951c7b4c77f4ff },
        },
        {
            .X = { 0x8f2b314653aced, 0x8a52e25d7942dd, 0x22a16f02d1a3b, 0x6b0fba82a0c29f, 0x6b25d79fb0f843, 0xa165488114df99, 0x4c415f2623a26d, 0x2d628835aeb8be },
            .Y = { 0x7a344a18bfd756, 0x24d5b4e87c3c55, 0x683e02b806b171, 0xd0464476ed307, 0x15739b1bbe7bd4, 0x9b1f005e0bcc00, 0xce8f0aa9a75c6, 0x1f52c41190555a },
        },
        {
            .X = { 0x829b29d3f26938, 0x142797a5e59cdb, 0x9ce5566222656f, 0x545c40f96e003c, 0xbac8df67b16121, 0x50d801dcedc5fe, 0x619c4ab82b4e64, 0x90975617dfebc8 },
            .Y = { 0xa14999dc1338f9, 0x92d1355cf05c92, 0xd05d5810d1ace0, 0x8d33ca1cc83383, 0x17581f68f9fbc8, 0x82704e885c98b8, 0x1b0276b0aba1d7, 0xb1d1b259ad495b },
        },
        {
            .X = { 0x6b46ade49ecd07, 0x8ae6192c029b82, 0x985af478ea01cd, 0xc667b3414d3f1, 0xb03ad6cfa29552, 0x37e54a767284f7, 0xf66df36aa63d29, 0x2e4548f8be41ca },
            .Y = { 0xa57de833e3c529, 0x725924766c37d9, 0xdfe75f020e0725, 0x78663e6c1a131c, 0xa01509a41107a3, 0x4dc15c5bddb8ff, 0x7859c1e2ccdd1d, 0x526d947dbb728a },
        },
        {
            .X = { 0xcdf43039b8f533, 0x758c4e7f02f282, 0x1b7d65dcf2ab0d, 0xaaaced061f24d3, 0x45bb6e72dcbe94, 0xe2e576a86c4f02, 0x302e47efff98dc, 0x75dab7494acbae },
            .Y = { 0x4e60b6ce4aeacb, 0x9cceaef81bd64d, 0xcb5d00040999fa, 0xe1c171962f7fa3, 0x557f31d2c39f8d, 0x20e7d940e02826, 0x839801b8722b57, 0x8263688a94ba4d },
        },
        {
            .X = { 0x11629068248e6e, 0xb9e0be41a9c400, 0xbff46658c9527c, 0x9ab0a9a217fae5, 0x9cbcc5cf92f343, 0x280cf1b16bafbc, 0x685bd99aac1185, 0x89a4980e2f4c88 },
            .Y = { 0x2445b5945f4f52, 0xa7fce0056422a5, 0x54107afcc38b76, 0x8391badfdca4a4, 0x7eaf33b1465c5e, 0x56e51c7668d3e9, 0xdee2615bf61919, 0x2a5e8385101c59 },
        },
        {
            .X = { 0x4164bc986013bb, 0xbbd9a07e47c25e, 0xf5e3da815f3c6a, 0x2140f200ae4e2b, 0xc09e9f42321774, 0x97656f95e77e34, 0x41d6d91018e6ec, 0x2885292af792a3 },
            .Y = { 0xa6bc290e650bd5, 0x2d9ee1a1a6e565, 0x8b522b53b9f20f, 0x282e7ce4baf694, 0x7fc6d802b29ec5, 0x4a7bd8a5485eae, 0x6ad0f71ba67edf, 0x8c5234a4328e30 },
        },
        {
            .X = { 0xb840913433e8e7, 0xa7bcc9939f78e1, 0xa34054c87eb0c2, 0x92bb5127a87bae, 0x8ff54009782465, 0x131e2881a9801e, 0x4140c78fcbb5c8, 0xddd470959c942e },
            .Y = { 0x6f65f677d39025, 0xd7955bc3b1b71c, 0xb4b42cb2db0a44, 0x95861f3cba6301, 0xd9ecd69c36b439, 0x9763383de3dc3b, 0xda8d204bd397ee, 0xfa8a3c3341a1b8 },
        },
    },
    {
        {
            .X = { 0x3bdb93b4f8b78e, 0x7ebf474cd4ed54, 0x80fa65ece72f0c, 0xde3524b92a8f9d, 0xd8511e44d587f9, 0x64aedb2c393ec7, 0x64b503c4554ea1, 0xc6d309947923a2 },
            .Y = { 0x189a788747eac7, 0xc5adc2447354e, 0xc4405a86c940d7, 0x3f5a7c8d1505d9, 0xf0acb77ce5be0c, 0xa00acfd939ef18, 0xb0fad43168e33f, 0x708f72b42c9a73 },
        },
        {
            .X = { 0x3393f0004f4b16, 0xa2fd9d59989fc7, 0x560a27ba0a6250, 0xaf1b31adff8eac, 0xa9384964a0ba68, 0xe5eafbbf6da655, 0xf217fdb31db5c0, 0xa0edb8318ac842 },
            .Y = { 0x9bdb4197afe593, 0xc5561ae87bd772, 0xb994fc2d1d7896, 0x93637664d746ab, 0xf7af9478c74059, 0x614083fe718c3c, 0xa51a2c19837cef, 0x57d6f90be98b91 },
        },
        {
            .X = { 0xf2318c25628c50, 0x95384fa2cdb96a, 0x5bd1ddfe0548d5, 0xc762caf83cae0c, 0x6fc1ec1cdcad69, 0x70532122553ffb, 0xeea8590535ec2, 0xd7d0d30bbda9ea },
            .Y = { 0x363b2fd4e60d28, 0x4d86938d3d9ce6, 0xd1da92c381266e, 0x1cffe9db28c890, 0x99e7aef6d0ade4, 0x540a66205ac81e, 0xf819bb45bae2bf, 0xeaf66d3b05bad5 },
        },
        {
            .X = { 0xe9bdefd8e1e228, 0xd5fa4ffd43c8b4, 0x5ce0fc988eca1e, 0x6540d96290809e, 0x553c1756f46c05, 0x66680555323f9d, 0xeaac09a06969e7, 0x5bb1d833013c2d },
            .Y = { 0xb9a021b174dccc, 0x77bd2bd4c44176, 0x5d578ce07f89ed, 0xa6abd82b3e36bb, 0x3a34153228429e, 0xd52fe5eb36714c, 0xaa0cb8a0250723, 0x617c3187d02643 },
        },
        {
            .X = { 0x598ffadc4dfec0, 0xc4660d28df0642, 0x3ef9e58c41d2dd, 0x11db45d1187712, 0x83f6610a9dc0cf, 0xd04bef7b72b8bd, 0x8809fa2f968cbb, 0x9922fdeb346c1d },
            .Y = { 0xa3d5b16e08e5, 0x552fe700e5fac, 0xed33960503285f, 0x45756d1a95b950, 0xbf3890707fa5dd, 0x59e00e958167dd, 0xecaf8d876937af, 0x991cb0b3b86411 },
        },
        {
            .X = { 0x84db20c978307c, 0xcd7c5587bde3d2, 0x34031c0a4bf1f2, 0x1467c81adcaa18, 0xbe6dd63b1af759, 0xb937358d03bf4e, 0x5d370f6275c1e3, 0x1b48323ebbe591 },
            .Y = { 0xc94a753ce15f6a, 0xf3c78924c1608a, 0x3dc552670db015, 0x3a3eb4a16a1e0, 0x609ba4f7c1fa3c, 0xf90ec8ba2e83a9, 0xc12cc3818d64fe, 0xc8481ec3c75567 },
        },
        {
            .X = { 0x379fae1fcfd087, 0x6abfab018d3d8f, 0x5e7f39da09bce7, 0xe40fdbd29a28aa, 0x5e243d39946ae, 0xa0d262998faffa, 0x4470c501b9801d, 0xf21dd49575a6c },
            .Y = { 0x2af337848e46f6, 0xb3c07235ce6a76, 0x4815ab96901b35, 0xe09871b97388b4, 0x648d171292823c, 0xdacbad06ff7724, 0xbc13b750f5ecf9, 0x5c09cb99673124 },
        },
        {
            .X = { 0x46e2c94a4f4eb7, 0x46f7190d3c6aa3, 0x284be3fa1f1b0c, 0x91d909167eb3ef, 0xe7c34b48c5a998, 0x7cfb6c532ccd50, 0xd118861f9f5fe0, 0x3cd0a2adc1a2ae },
            .Y = { 0x5d717e87dd30bf, 0xa8e14adfdcc314, 0xfdafd083ec9cda, 0xf4ab40da6bbd34, 0x69b85827826ee1, 0x4939b33291f0eb, 0x45ed715b05b243, 0x79442a84291c2b },
        },
    },
    {
        {
            .X = { 0x17e79a0a9f97e9, 0xd4c3fd8c2ee770, 0xfdb3e712fd5eec, 0x2546bb7d583581, 0x6354b00bd02dbc, 0xd1bed3b64abaf0, 0x55d533ac5a1119, 0x5b3c905d7825dc },
            .Y = { 0x3e1be8e9598bf, 0x48d64c469b70b5, 0xf1e270ace5b1f3, 0xb39322e4970372, 0xa2e7eeb6574749, 0x5ff873d39091ba, 0x45c37d870cb285, 0x6baef332dfb3e6 },
        },
        {
            .X = { 0xcbff257fb672fb, 0x7a7fa274b76d0f, 0xa8e4afedc438c3, 0x2a17d0296b29fa, 0x6eb28cf2288bca, 0x61a9f6288c4255, 0x2a5459d2a94bcf, 0xa7779d89abe2d },
            .Y = { 0x9c8d6969c557f9, 0xc84ccc495f462, 0x316eb918c22423, 0x3e7cd2e5ebe2d9, 0xca4e43f4aafa9f, 0x7fd244cf825196, 0x8c6f5c9d813995, 0xc4dd09e783b008 },
        },
        {
            .X = { 0xf20968bb187fb4, 0x4118b9a1d957f, 0xc381968f992664, 0xc17ffa0592a7b0, 0xe12d3efc55d022, 0x7410a43915e7e7, 0x402c841a0e379d, 0x1dabdbfa61cb13 },
            .Y = { 0xf39bfe5943127, 0x585c433c6377f9, 0x7b52ebedbd5de2, 0xdae232ab4ad400, 0x43cea15a8454ab, 0x1203c16ef090de, 0x64edcd81ac96d6, 0xe02361feec2dbb },
        },
        {
            .X = { 0x7a91b38ee9594e, 0x7008437b30954d, 0x7f66f8f25fe40c, 0x240fc56120b459, 0x86051cba28c6a0, 0xd7aef1832f9ff3, 0xb7bb3765457fe9, 0xd8a35ac0dd83f6 },
            .Y = { 0x3add111f50e6fd, 0x5528ada293fa5b, 0xc7f0ec67cb59e9, 0x597f6080b05289, 0x99b15e100727c0, 0x53bfc77d5b8ac8, 0x9b559bc2d45d91, 0x770a9c2302b822 },
        },
        {
            .X = { 0x99e0379fb0f0d9, 0x39ae18f33f735b, 0xa64ee939841430, 0xa0549b1b715e70, 0xce769bcfa2eac1, 0xdb8bda70ad05c9, 0x8f6b84fa56373c, 0x95d1592c2d2ecd },
            .Y = { 0x27b82fd0430aca, 0x7539200e5d8212, 0x16d2941525c4a6, 0xb242505f8cded5, 0x84d5563f298b63, 0x5bf5a6993e80c6, 0xaa841add02d96d, 0xdf224b5dc8ce74 },
        },
        {
            .X = { 0x51b6b4c08e88ee, 0xdf7ac0bef2426d, 0xba6e46c73e882f, 0xe33387b7c9839f, 0x4c33f8b53a7b8c, 0xfd17b51e3ec762, 0x80ae097ce7b140, 0xc5ea628e9ac905 },
            .Y = { 0xc8ebf67f094685, 0x9c3ed241f0d43a, 0x4cc787d5e124ab, 0x541c565e79af4e, 0x58162d0d6a94f9, 0xd23a4bd962563a, 0x38d9ff7d1cd8de, 0xf6c9d5cf75ca6c },
        },
        {
            .X = { 0x7356c857682f2b, 0x74f6071c5191e5, 0x8b059245cc0002, 0x620bbc23b0ccc4, 0xe8e55179f43ddf, 0x67e26f86335e85, 0x8348b68f4adfca, 0x190ba7d13b2930 },
            .Y = { 0xe19361d6e5904b, 0x338e0196db9d82, 0x35b696ef3c08bc, 0xe940f185bb522f, 0xac520a86480bf4, 0x58571179c92ec7, 0x4c5a9afc3dfd14, 0xe41e345e84b9b1 },
        },
        {
            .X = { 0x4dc19071efaca3, 0xd4ed31f66b6750, 0xf47fc9b773769c, 0x4ba9f61627f752, 0x4f3c3b94b4ff9a, 0xa85712d6f83c77, 0xc107b306fd9119, 0xe7071a72b0b1d6 },
            .Y = { 0x5a21b10be1c6ec, 0x40d4518ba11b, 0xca2b6460526ce0, 0x5949086a518ce6, 0x9048dc2f13136, 0x47c0bfb29acaf7, 0x924598c6347120, 0x7313ca242fd7f6 },
        },
    },
    {
        {
            .X = { 0x100893805ffca1, 0xadc4929e69b1e5, 0x8ea42b64894482, 0x691dac7bf0e5c2, 0x2392351745907e, 0x3a812fd07db228, 0x77a6e6ef7c9736, 0xdd926dc2c137e1 },
            .Y = { 0xa5b6afcedfe7c1, 0x50657db61837b8, 0xd94527f932330d, 0x209f32e0b4d929, 0x506c1b702e6475, 0x48c8272eb56e48, 0xd2bba620a20909, 0x132f7d1ea19ed8 },
        },
        {
            .X = { 0x589472e6971678, 0x8f88a7336a9a26, 0xb1a6bae7dfe196, 0xed50e4346c09f5, 0x822855ea3c0c24, 0x7387e0be750289, 0xa7cbb4f95a98a8, 0x3fe14b6d107b62 },
            .Y = { 0x7f3cbc45317a10, 0x89914b7f4a2d2b, 0xb1196b3114f563, 0xc1b2579c45ce04, 0xd11117efdafc4f, 0x108a47eafbb192, 0x9bb335c875bc1, 0x8cdb21c37f0898 },
        },
        {
            .X = { 0xd67d4bbfeb9a15, 0x36dd806dcf5299, 0x40ce1fe17e656e, 0xc04b98d33845f5, 0xb7ecc66f775d2b, 0x5b1cc80ef45860, 0x4850029086e3e6, 0x92158a00a22f84 },
            .Y = { 0x69a2aee3876055, 0x492a81a8ab9529, 0x3d6061eee39747, 0xa36d97c4f3dd2a, 0x84f067059799b1, 0x562cee8c6cbb0b, 0x1bd48f1c4f487, 0x5742e0a16251a },
        },
        {
            .X = { 0x38059a6f5086d0, 0x35f1eee8dcc3c8, 0x805fc35c19c2b1, 0x7feeb3a576451d, 0x921a422856abd, 0xae0d7aa5fc33aa, 0x3e726b69debb53, 0xb75ee536636b22 },
            .Y = { 0x95221db986e94a, 0x592b4a84e64541, 0x9be278e88c8c4a, 0x20519b784fd4e4, 0x66fc252cdef20e, 0x898d2c5b345186, 0x8b671c3d870654, 0x1dde810916232c },
        },
        {
            .X = { 0xf09c2c497d3fdc, 0x143f77847d0731, 0x72d3e98399d692, 0x65d5c7b148c777, 0xa90865c089409f, 0xf61fa69a719784, 0x38a5c71616c8c, 0x518a775005975e },
            .Y = { 0x5fa0b6abd57959, 0xcc8ca93e4a262e, 0xa1a25a84253e74, 0x9f522cd0b479a2, 0xe3afa34cf97d86, 0x50bb8a6aa121e7, 0xdc960b1600fd31, 0xc7c49ddef5b592 },
        },
        {
            .X = { 0x7c53cbbedcc3e3, 0x59264a2b59bfa2, 0xb56abf016294b4, 0x31c852fd7d9cee, 0xea17cf2dc128ae, 0xdad23b15de0407, 0x2ddf76ad686ad, 0x5a8f3c3a18a864 },
            .Y = { 0xaa48ab1c3cd94c, 0x41cd46afff2a11, 0xc76b11c0eeda72, 0x26ca71ecbd231f, 0xca1b84cc627815, 0x2c5dacdad02da9, 0x1818d757f7de9d, 0xf2387e203d3e4a },
        },
        {
            .X = { 0xef0cc2be141316, 0x40508640d26b93, 0x90063d181d8240, 0x48a0398188cbf5, 0x20b5c1f7ffc8f7, 0xa146ff383d8eb6, 0xdcc6a0caf2f189, 0x8b2915a679c598 },
            .Y = { 0x54b55f0007c0bf, 0xc9e7344536c364, 0x8274ea7a319e9d, 0xb670bcc84caca4, 0x5c1d693dde962, 0xc1263fa1342213, 0xc342e22b617aac, 0x624786836d18b4 },
        },
        {
            .X = { 0x30927b39cc70c1, 0xc1337136858f26, 0x507d822e3bd6cb, 0xb59edeb142e92a, 0x69a990d94b5b4e, 0x726ac1bedc0498, 0x7b8c7a977a6951, 0x3b698a36755cdc },
            .Y = { 0xc217db19d9b70e, 0x55d70e3aa2ea04, 0x3dcfa9eb2f292f, 0xe37a10b7b477e5, 0xf1d80ba9eba6c2, 0xce84db14dfe5e7, 0x70b485933009b2, 0xb6043f83ff7a3d },
        },
    },
    {
        {
            .X = { 0xfe5850453f3a04, 0x5c233c2d5d812, 0x8dd8c3cf20f057, 0x88605668c4b87f, 0x348cb221415546, 0x63af37a4851069, 0x4288eb3e47c93b, 0x8c78d325ded4e },
            .Y = { 0x9ef5783082a37a, 0x5a1e7c3dd47273, 0x2eacdca6e77f4, 0x39c9a9cd83b46d, 0x29d4190d39e38, 0xf9db218417fac, 0x4b8082e7e91c21, 0xcfbbbad22a456f },
        },
        {
            .X = { 0x754e8c74589dcb, 0x655d370c3e87cb, 0xfcefe3ecc79fcf, 0xc0d7541d3fad20, 0x8ca0ca1d589381, 0xa68d0231a10a79, 0x6db942a3b7682e, 0x3851eebac1f46a },
            .Y = { 0x59f39991f124ed, 0x3e295affe86fa, 0x38aa1265b09b03, 0x2dae79b2e65aba, 0x5a482109d18b43, 0xc2a47df007bba, 0x2b63f0dccaf7c4, 0xf267ab4e7c839 },
        },
        {
            .X = { 0x8035ded9461017, 0xaf02b3d02b420c, 0xe48fd9c040ef6c, 0x1b18bea0b212de, 0x3ebfcd2d702fd8, 0x93c16bdb2110a0, 0xa390ed8ac94c5e, 0x99d7fae299b233 },
            .Y = { 0x6690741892b093, 0xe0e9324c65bbdb, 0xb0abd898e86e86, 0x8b629d5d250ced, 0x5c4853b170bca8, 0xf9faa1ba7409d0, 0xcbf2847d03b3b, 0x96fd4f2e95c064 },
        },
        {
            .X = { 0x63fa3217105f7, 0x55c7eac777e04, 0x3af9ab2c9b9e60, 0x2138cf961c376b, 0x90f497035c611, 0x2196465ce116af, 0x698a2f7adcd1e6, 0xaf2a608e40e90d },
            .Y = { 0x7ef3c9cd97c5da, 0xb8813dc7a9b300, 0xb02b28cc9b76d4, 0x906480a697a986, 0x53eba79751c895, 0xa73eeee35260fa, 0x2857fca281de55, 0xcc9b978f16f8fd },
        },
        {
            .X = { 0x32fe1d1f57403a, 0x583dd91002f035, 0x4ca6938ee49997, 0xe1ec1d001c5554, 0x8baf2efbf8834d, 0x571be7d5e78404, 0x9cc3ed22af961a, 0xcf457437d5d05c },
            .Y = { 0xc08f42af03b6cd, 0xd8e50baa3fbda2, 0x9f75d2d02c8d3f, 0xf61409f260409f, 0xf5433849e6c0bc, 0xdbf231301545da, 0xc6f06fa4e077e7, 0x5a58f00714d8c9 },
        },
        {
            .X = { 0xd1b640dd052ba0, 0x791850e3f6ba60, 0x6528d2fda4d379, 0x2a577c135df32c, 0x3600d6163d7c69, 0x2cf65e273e8883, 0x7c7149a9a3856f, 0x61cfa381608900 },
            .Y = { 0x7c5d1e0f7b7899, 0x8f9186b1474006, 0x904b5ee3de0ddb, 0x9a4e0824fce468, 0x7304ec1047ac77, 0x46f64f7099e71, 0x1a53d78cadbc52, 0xecc58628b808c6 },
        },
        {
            .X = { 0xc0b5bdfdfb3875, 0xc0a4610034190e, 0x9d29e777758d41, 0xf27e034b0a281f, 0xaa673727bd66e8, 0x8d4d93ffaaa01e, 0x7cbc3d0be95b3a, 0x2dddfda4e66425 },
            .Y = { 0xd35b4960971dbd, 0x56cc3056a56ee7, 0xdce34ccb296c53, 0xc580d354593efb, 0xd0793caf71e97e, 0xfc38e736e3cf50, 0x642cd925f47508, 0x1d701ac0e258a },
        },
        {
            .X = { 0xa436145c208782, 0x30375869d2927, 0x1c603a8ae281c4, 0x500ad5dc22ca9, 0x37ee6a93a65a12, 0xad516993af26d3, 0x7e766d9187cab7, 0x65cbae4394408a },
            .Y = { 0x4dc7c49283ef8, 0x115674d88408b4, 0xa07227bc1b8145, 0xbd5d269e62aee5, 0x4f58fb747e804e, 0xb13dc940f79e86, 0xdb068cb2002189, 0x3ac39a4f4c894c },
        },
    },
    {
        {
            .X = { 0x8203c8bdf4ff3e, 0x8b6c0f8b79b33e, 0x6ac769a0b4ab23, 0x646d251dce75db, 0x5239a7b2fe48f2, 0xfe70ccf4b7adb1, 0xe2e1b9a1efe88e, 0x433b7ae4ca154f },
            .Y = { 0xd824ac3181b2ae, 0x12b72c331906ac, 0x60846728b30801, 0x35d1b272062611, 0x26fb6d4d7003e6, 0xd898baef4e5ca4, 0x3a99b1b0e3c7b3, 0x2f5d971587aaa4 },
        },
        {
            .X = { 0x735074033bfd18, 0xff2284149568af, 0xddb19450c8b944, 0x579986c99bd3a, 0x3ad5722b295e9c, 0x43f7a338065059, 0x3762312b86441f, 0x91a5233b0aafb3 },
            .Y = { 0x55451392f9547b, 0x1a7ec3945a5d0f, 0xbc19b32fc0859b, 0x850eec0e8fe2d4, 0x2629e07f3849b8, 0x3d48f6e510ab4f, 0x4471221ff6bb1, 0x5a19f74298c345 },
        },
        {
            .X = { 0x334b45be5b4228, 0xddafc206995a9e, 0x1188e554eb5180, 0x668b31e191d93e, 0xd6ee396804ff64, 0x2dd9f4f0087ba5, 0x5fddecb0f85e9c, 0x64a7a217e86e31 },
            .Y = { 0xee4621c1a39e36, 0x57e68df2827266, 0x2d012458692e51, 0x78d03d6ca0642a, 0xfd8c218bcbdff7, 0x68ac321f75ceba, 0x3f8a8bdf35a2c, 0x4dafd85763a767 },
        },
        {
            .X = { 0x835f62c9914616, 0x8b4784368667ad, 0xb4b565ec33b9c1, 0x70a90dea5831f4, 0x1c85874ba1218a, 0xe8e5a59f25522f, 0x1986c30d249c4, 0xb799ad64385f3f },
            .Y = { 0x6a4d300b3d5562, 0xdb1d42bbba56be, 0x8b580034743a90, 0xd4affe33df7d0b, 0x4bafbfcc8acff5, 0xd9c3e043919efb, 0xb23bd449b0eeb5, 0x80957c9edd472a },
        },
        {
            .X = { 0x239be10fc9b729, 0x9a8dbffdd6f4c4, 0xfd16d4f4ff6d97, 0x64360acf836231, 0x6a050048f221f7, 0x3c3d56da569d82, 0x620d2477a6365c, 0x519c0733dd6028 },
            .Y = { 0xe8e06e00473fe8, 0xe1e2e84c3df159, 0xdcbe29d685abf4, 0x230a4c74cd7208, 0xd35ab0cdb78786, 0xf99e628773044c, 0x8d06458ff63c, 0x5da17ee63f7ec7 },
        },
        {
            .X = { 0x8a5dd6b81d8c83, 0x80977e41a259c5, 0x7f05bf8d732d80, 0x8918c4c4ccb7cc, 0xf7e52d846daaff, 0xefaa19f4c9da2c, 0x30729edfa2ef63, 0x4251a0d50e71f4 },
            .Y = { 0x56fb4461151622, 0xf18a0ef81acaca, 0x635c6d72b4a53, 0x52473595502f5f, 0x74fa94a31eb9ff, 0xc70f3b855b48fc, 0xa7a4dd35347907, 0xf3fe7787553b91 },
        },
        {
            .X = { 0xeb968b2a5aa3bb, 0x76fee33739a245, 0x7933e7ad771c7, 0x9f801835bd175b, 0x83d87b52765de8, 0x9c43065d8a19e1, 0xacc4ecfd9be5f, 0x7964a859057f13 },
            .Y = { 0x444e662f9b52f9, 0xca4d67ef315446, 0x604a0504d94e0c, 0x563aa7be45ffc1, 0x93825444e5a296, 0xe7dba7a8fafdbe, 0xb87738ec708aaa, 0xc6d3c30ed972bd },
        },
        {
            .X = { 0x4452d8895cba6d, 0x53e7771a3ad3a8, 0x8a938bced19244, 0x9310ae66106d22, 0x61b22cfc5f1fd1, 0xbd1f4809fbe161, 0x9866c4843bc034, 0xda8632b2afda54 },
            .Y = { 0xe776fc121a3cac, 0x1ddd146c4ee1ca, 0xc12fef63bc4544, 0x6ad7fd7d523f5d, 0x12f047fbef057, 0x55bd5208c72c4f, 0xf1d0a0c594572, 0x1a8f48c757e607 },
        },
    },
    {
        {
            .X = { 0xbb43086502774e, 0x51b249067e4ca8, 0x2151d46b97ed33, 0x8f5fcf32a6e641, 0x3a788f541c993b, 0xf0fc0a9dae3a0, 0xa15fd5d10eb69b, 0xb8b258f0b23cf },
            .Y = { 0xe3a49d3e402f4d, 0xe911b8d4d9d9ee, 0xf9ba863e5fe866, 0x43c896a0c8e9df, 0x735397043e9669, 0xe166e8405cea51, 0x1ecfb8ebac2952, 0x22adc0a596bc42 },
        },
        {
            .X = { 0x461afdb1d2564b, 0xf30f12e14b96f4, 0x2ae47deb7d003d, 0xa7112a536b5283, 0xd59f470fea0ccd, 0x36f69f2bc17695, 0xa6830e220ccff0, 0x8678a5bd0ab43 },
            .Y = { 0xc428fa281e4896, 0x7bb8973e2d6fa3, 0x3769b5c670c595, 0xb607d52bc9ef42, 0x6c8a3e03e7e678, 0x7e2f8681608f78, 0x17cb8571d234d, 0x8e5d81918f6226 },
        },
        {
            .X = { 0x7cb5607773eb6e, 0xf6d4ba034b3348, 0x98ffc5342944bf, 0x35a5c8bc9b9317, 0x5bb9b98f1aac99, 0x3b01547e8b9965, 0xcf8e7ffa40d0d7, 0x6e7ef6b800211f },
            .Y = { 0x21a3b44d03840, 0xae608aa3fc167c, 0x5a73f2cf73ce1e, 0x78e4904deb086c, 0x747e4be0c3f1a5, 0x5944e9ec551ce3, 0xfdc70b12b8fc5c, 0xd841a603857345 },
        },
        {
            .X = { 0x1913b78920b732, 0x8bdee1c62c0830, 0x2de4e42ede8807, 0x7403a7c0207c35, 0x8dd6502ab6868d, 0xd757d281064586, 0xf753c090eeb2dd, 0x425487b8e8e5bc },
            .Y = { 0x45ffc763527201, 0x5c95e2f7056697, 0x6261694c5233f5, 0x49606af81f3542, 0x81f98b8cf44624, 0x7c6abe96d0eb3, 0x999688876381b3, 0x7f31f3c517b6e3 },
        },
        {
            .X = { 0xd2ad61a3b2e8ca, 0xa54eaeb247edf1, 0x29e1f6c71b6d23, 0xd4b311cd0d4ba5, 0x23bdf51032e6fc, 0x1547ad0026917e, 0x5b4da0cae0c14e, 0x50381559fe708f },
            .Y = { 0xd7002080c85d4a, 0x215d448988b5f1, 0xe672ffd237d66d, 0xbf10dd5f27f0dc, 0x7bb81ce6ed5887, 0xcd52ee9061535d, 0x4940d2b522f064, 0x5db432148dd301 },
        },
        {
            .X = { 0xb54862cf74d88d, 0x7b71522af1e07a, 0x338ff86ab85774, 0xb28bc80602f79c, 0xc801830f502ec0, 0xf07e11b9553682, 0xd5d941d166d6aa, 0xd25fe5cb3504d2 },
            .Y = { 0xd9f22b0dec1ee, 0xf3def305c18706, 0x7ce253ce690fdc, 0xcdfc4e0dd3ba8f, 0x10e544fb159152, 0x3f5a919943ce15, 0x1a7ae922255c74, 0x6530392ba14ccd },
        },
        {
            .X = { 0xa62025876e682f, 0x9cd0b0d64368c0, 0xd570ebd57fae2a, 0x3e2db83468837b, 0x1349150c049e01, 0x98465d7f83b696, 0xa801e73f3c0991, 0x534cde5233ea47 },
            .Y = { 0x6415356c85d779, 0xa484e14723072, 0x6f1d0042301543, 0x464ca2a8f442cb, 0xb04191131e45c2, 0x3b8962a4be7392, 0xc9cec186bffd9b, 0xbccb7043405e55 },
        },
        {
            .X = { 0xb29d1286d27154, 0xa997d6e6288392, 0xcf7b24b63178c2, 0xac1f2cf635907b, 0x6861ebaff3ac61, 0x86cc1c8ffa7d06, 0x94f39f5161022e, 0xcd131c1dcf3921 },
            .Y = { 0x2ddc4cdd42884b, 0xa49e46437b973e, 0x363d5ba6d8aeab, 0xd6f7d6dfab5086, 0x4043cdfccea400, 0xb9fe0a6603b687, 0x61bcaeae7a6e20, 0x4cb77169218582 },
        },
    },
    {
        {
            .X = { 0xc1e672a8d11892, 0xd96e5ae6e32b30, 0xfe85c90c263e2a, 0xf5c8a1c2248e9, 0x6ef01b5dbd3f1f, 0x910f023046a1a3, 0x1e27d39fcaec1a, 0x7a853cdd7d38ec },
            .Y = { 0x5bd77ec1e2d277, 0x7146f18359bed5, 0xc34cf0d910c008, 0x20492d99a31dc1, 0x1933359559957c, 0xa3d8c8f0f5f7c3, 0x1102e424a3cc79, 0x1afc8fbfbbc475 },
        },
        {
            .X = { 0xe1467b030766f, 0x894ed718adadb1, 0xd7a333ab9476ed, 0xe6e9ddc410346d, 0x212841e0d5eb3d, 0xe149025a03c62c, 0x5107dd935e07eb, 0x4dfee78d4b3a1a },
            .Y = { 0xad1e246419b7e4, 0x87f916811bd270, 0x1a808e55f4e038, 0x9c2e44cea61ee1, 0x4f5fc32a3ca9a8, 0x2c854b5fe1f793, 0x7a464a7da3976d, 0x4bb7edda07a7c9 },
        },
        {
            .X = { 0xf80a463ab7712e, 0x9a7bdb97e980fb, 0x6c08762cfc740f, 0xb9c1580abd61d4, 0x7883aa3cef07c6, 0xa381c9c64079e2, 0x815d7e38b86832, 0xed5f82e52eb597 },
            .Y = { 0xa92e6da4454ea7, 0xe3672c08099dbf, 0xf917eeea057930, 0x1ff9606119feef, 0x8da3224e79ed5d, 0x2b16457da3e50a, 0x5d4a3423e53580, 0xd3f6af8b484b7a },
        },
        {
            .X = { 0xead578f71dea7e, 0x49e07f21c1ae7d, 0xfe423ee4fe78cf, 0x23ed9d8dee1597, 0x1c08c105769b86, 0x133316c3b29778, 0x78d68cd4137787, 0x46ec1a498e4f1d },
            .Y = { 0xf29adc162ed580, 0x2129a5a63c7f24, 0xa59685028580fe, 0xaafd0b86dc10a5, 0x8ef41170e04e88, 0x392c72033da494, 0x2b3b33da4f0122, 0x73c30352799bb4 },
        },
        {
            .X = { 0x1380e699ed642a, 0xabe021d7cdab, 0xe6add2f2d03588, 0x245b18d9872b0a, 0xffaacfe2f329d9, 0x3cfb326ae75c20, 0x45dc727349d822, 0xb7f8a2c6cc5c68 },
            .Y = { 0x1218b302942427, 0x961a4fdc77bd10, 0xd644ad0a41bbc6, 0x3f373fc5d39e43, 0xa2789dc6b0bbb6, 0x7dd174651d0253, 0xba475f5f02effb, 0x3f7c054399b5fc },
        },
        {
            .X = { 0xc88df8a442b87d, 0x8f6ab713eb3288, 0x939a0691e67105, 0xe627216b4567a9, 0xcec257762238e2, 0x4eeb862a4dfd11, 0xba0641bd715d1a, 0xa4253b82bd3f2c },
            .Y = { 0x3f199c1bd61533, 0x7dc5880b647c81, 0xbde0e432b50679, 0x7b0a362ddf66db, 0x652e8ce2cbce1e, 0xa0afe6a92e2394, 0x8864810649eccd, 0xf0e543dced69e3 },
        },
        {
            .X = { 0xdd2d92544e7cd, 0x3c830cc6458179, 0x890dfa8e70dbbe, 0x6d2ce9469ec018, 0x9e9d35ad0a3788, 0x9f0c298b9bd786, 0xd2b15caffd56c4, 0xb2aa5337777e7 },
            .Y = { 0xe745a83e9c559e, 0xd98f7c7c00227b, 0x1d7983b9eecb54, 0x17115a92cc6ea4, 0x842e2db9e29461, 0x8c404493927a54, 0xaf19957c85499c, 0x52aa700ba13652 },
        },
        {
            .X = { 0x49a931dcda195, 0xe60ef70896045d, 0x7379411e1af537, 0xe15ae0b99bbbac, 0x1cdd79d553eb17, 0x41f3b0b9327ae5, 0xdaad09125cdb04, 0x46d3fe6a9cdafe },
            .Y = { 0xcb870f3e26b3b4, 0x82876cf2523931, 0xce7a31da917b0, 0x9e508562c5eaca, 0x3807cbac035fe6, 0xa8ae65a8c2c029, 0x54654ee535e30e, 0x4e8df2e3eb6a39 },
        },
    },
    {
        {
            .X = { 0xc4bf80e20c2cdf, 0x2e6307f972ee7a, 0xe4f3a3060ae4c3, 0xeb5d17b81b8dab, 0x7e9db9d2486fa2, 0x2d517d4ec729ef, 0xe45fc972ca6da4, 0x41d4def0b0e043 },
            .Y = { 0x1dc8aa14ecd979, 0xfd21e56a6b5867, 0x120f6f4d15f24b, 0x2411ae0ce601ab, 0x8d637edb7d8c52, 0x8c8cf22cec9d4e, 0xedfebf16da0a35, 0xca592ea8d156dd },
        },
        {
            .X = { 0xf85f539fc7abf3, 0x38461e7aacf4ca, 0x5dd3abd2ca0e1b, 0xc06676f3fb5a58, 0x373e78c08b1502, 0x18d73238cabb69, 0x20740e9a8e57b9, 0xdd92041ffac84f },
            .Y = { 0xd8f76a95ed9811, 0xde94f8f46f2502, 0x7dd9beec256c64, 0xd71d58d9ae168b, 0xde4550762e40ab, 0xd29d92abfa8e6, 0x3aa294e340682d, 0x9b9db10ac62ac9 },
        },
        {
            .X = { 0xf0d4da8a7d2ca1, 0x43ab7f1dd87a61, 0xf74f8902573841, 0xd8f404db947c3e, 0x483dc9df5cf68f, 0x7ca26d3f210bde, 0x3038370a95ae6c, 0x316cfadd2f0df9 },
            .Y = { 0x2a7b44bf225a2d, 0x23ecb7b1a5b21a, 0x8f943effa10ecc, 0x3ad7ce0e90abc5, 0x7d7a9bc82a9fd1, 0xd2d836f4acccc9, 0x6a040fcfb0367c, 0xe688d9f9b73d8b },
        },
        {
            .X = { 0x5a4fa44e051c04, 0xed74fc35355ffd, 0x548dc5c78db3af, 0x6db757174d3375, 0x5f784bf53ad3f1, 0xa50c34eb0d1716, 0x8c7b6092cfa9f8, 0x78aa48c8c2c152 },
            .Y = { 0x471d5bc68ea73, 0x633b9289ca52f3, 0x1abccc3aa852a5, 0x43b7e6ab86b719, 0xa55023caa65577, 0x2fcb0ea8e6b62d, 0xaf8f117fd8704c, 0x3b270c77507ab3 },
        },
        {
            .X = { 0xa0e525497d8169, 0x9cb8b20522c32c, 0xa1b91a06a91b3a, 0xd32a740d5ec6fa, 0x2ff10c2ee49c62, 0xd0bb9887eba749, 0xc6dc317fb5c494, 0xb0e9453a4a26ef },
            .Y = { 0x984e1516876c8e, 0xcab1c3ca07d384, 0x140294c3623f44, 0xaad6fc8437c1ae, 0x6dc7e0bae5e49, 0x37ac602c27054e, 0xd4d7dd3c98574c, 0x19693be5dbad9d },
        },
        {
            .X = { 0xa203931e0d5fd2, 0xbd8e44da14bce, 0xd49eef34affcb6, 0x6c650fb5ad6431, 0x8f06709e1b5b65, 0xde9e70374c1c34, 0x7d59b8d2718840, 0x82a740402970b3 },
            .Y = { 0x9b7117c56c076f, 0x6c9b2934da3f97, 0xa6b52b819e7519, 0x39f93f9d236ed3, 0x3b659d93374e93, 0x3488f0c2ca1646, 0xeae2a9ad13812a, 0xe0071b8544eb58 },
        },
        {
            .X = { 0xd759cde1c7c4d6, 0x71c1849b13e9f8, 0xce0a652221f4c2, 0x14e2c3a6980908, 0x5934174e02187, 0x4c54b8022cbc3, 0xf27d429163eedf, 0xacf4b06c7bf33f },
            .Y = { 0x35b50f86abe8cb, 0x7e16e717601f90, 0x84b9369a42aa42, 0xdea4f523b42bda, 0xca646aeea77ec0, 0xfda46fe84d6f0a, 0xcb86a8e8303994, 0xce87beb0e5cbfa },
        },
        {
            .X = { 0xb7cd8bc8728dfd, 0x86e78d7908465d, 0x1dcffe7d7d1125, 0x840548ce0d958e, 0xbb5245ae12bf78, 0xf34a1c8eeeb59a, 0x1bc8c0e3a3ee6b, 0x6dfb056fecdc15 },
            .Y = { 0xe60fbf0ca65ba4, 0x3104647be4907b, 0x36ac7e80c1e729, 0xf9010207faa673, 0x4acd28652c21ba, 0x1ba8c47bb1d219, 0x55a1e118ded4d0, 0x8c0ede9489d7a0 },
        },
    },
    {
        {
            .X = { 0x45b3a6bd9185fe, 0x8e4b2157c425b4, 0xfba9fb09aa223, 0x62a9aa592bda9, 0x27886ac51c6a75, 0x7c07964ca90d1f, 0x7ec5bf61c53d87, 0x86dc8c0f1fb2ef },
            .Y = { 0x38039487d9ad89, 0x763d363c795f5a, 0xb3f64423cf072e, 0x9970cd6d03e2ce, 0x120606c3f179d0, 0x1b5527ad89c791, 0x626c061db39fc4, 0x352c37a87970af },
        },
        {
            .X = { 0xe9e988d9b84794, 0x7d0985ded200b9, 0xc31d82983ddcef, 0x56364cfb7b0d8f, 0x31765923b9670b, 0x5b6793b5aaafa2, 0x46240bd6e22339, 0x23f88df1f3355e },
            .Y = { 0x9768d447a97547, 0x439aca17b5880b, 0x1bc48e9fa9352b, 0x7f00c234c3a4f9, 0x4c0640400aa777, 0x7d7d6e02b3d766, 0xd1eaa1683896c1, 0x2ea0cc0e5603ca },
        },
        {
            .X = { 0x9251c3f3cf89e1, 0xfb90b40ff59091, 0x436e6f50bc10b7, 0xebee56b8d6950b, 0x4a1e9ab0dfbe98, 0x70d282b12824e6, 0xd1de1e4b114edc, 0xe093388c67c2aa },
            .Y = { 0xbfeeb6105ee54b, 0x45eb03d50fa362, 0x35e309f90116de, 0xa9b976c5291a0b, 0xdb28c0154e30ab, 0xeca3b38c28da76, 0x982a9a7ef22119, 0xf0b16a3948313c },
        },
        {
            .X = { 0xc29934720b3dc2, 0x9d82ed588759e2, 0xf2ef03f05a2cda, 0xce2d10ff4f3a1, 0x6cf9a5048c6c99, 0xe83c8c36a479ad, 0xb60ab4c26b7c22, 0xfaa78b6bc8894e },
            .Y = { 0x29192452774bb0, 0xe692c911007629, 0xf79f2c3cd4ca57, 0xd2464d2b225dc7, 0x2c0bfcf507953a, 0xa38599ebdf5136, 0x69c2994a1edaa4, 0x66705c6d87703 },
        },
        {
            .X = { 0xa9a48f21a90210, 0x2d7f4e9a5659dd, 0x8e95b875de6a77, 0x2e56c308af3b45, 0xf52e0dad5e9c7a, 0x932caca54794e3, 0xba580c20b3884e, 0x436519b728a2da },
            .Y = { 0x57c045c23b19d8, 0x27d35ac79e2ff1, 0x4c6ace1b32c0d0, 0xb043a88adcb6b1, 0x7a07c8eddd908e, 0x70c0d5c89c6765, 0x6e447e05a7888a, 0x8cd14f02b2ba51 },
        },
        {
            .X = { 0x3a6323fd5b83c5, 0x21856f66e83e74, 0x44fa8d9f6bbd84, 0xe4657653af1e31, 0xb03cea9c41f78c, 0xf407a962d5cf0e, 0x724fb513d86a0a, 0x8bb8bbd8e6dd03 },
            .Y = { 0xc7c8af9af985c, 0xf3481092c85b3a, 0xede1b2ceb35ea4, 0x803ce76ff89b25, 0x21d6612cdfa839, 0x598223dea2d2cd, 0x3a44f541c92563, 0x626a179b9b1349 },
        },
        {
            .X = { 0xf00b365522efd4, 0x338ac83e271672, 0x39c7a6735ec9b3, 0x86e233fb278c65, 0x6278fd8119415d, 0x95e85d5eb9c801, 0x8d7d0fbde19a87, 0x5dd9ee2095bf36 },
            .Y = { 0x6d1b7811e98a54, 0xece41f243e4937, 0xf5eed6d19ddeef, 0x3b02ac9314eb4, 0xee55ad061e8274, 0x3ff2d4cd2ebead, 0xa9fa5f744e6c92, 0xd21025c1adecb2 },
        },
        {
            .X = { 0xe3687c25015bf, 0x39d40a683c1547, 0x3a77bc2ee1962, 0xab5b48eff0b28f, 0xdde06834849de9, 0xd7783f7f7dd532, 0x1e8bd88bf4bf52, 0xbfaccbf17dc71c },
            .Y = { 0x9297df55843386, 0xd790f60be7f1c7, 0xc7ea15832f9f60, 0xfaa3937e7bdc0a, 0xefdec611d46fa0, 0xaf3f80094da4d0, 0xec989f72274a29, 0x4da81fae4a2697 },
        },
    },
    {
        {
            .X = { 0x15d8506bf9ae3d, 0x8d155d18cc0d7d, 0x94e6acbb9dbc66, 0x9f9184957b11fd, 0x1055483e141581, 0x4e3d5541982a12, 0xbe4c0f10d72f57, 0x9311751f625bd2 },
            .Y = { 0xcfdfa6c60acc33, 0x51353b3c862a67, 0x7f22b4238c8869, 0xf55e8ced02a666, 0xf8c11a991a2590, 0xb20eaf1765fb01, 0xc97cafc64df6f4, 0x7629e74596eecd },
        },
        {
            .X = { 0xfb20d22b84a80a, 0x4f224568cf6d5f, 0xbb1fe9fa68a38a, 0x6d0165dc758b29, 0x50c427ed6e273b, 0xc0cd1119c22e2a, 0x342ea26b7ffb45, 0xa0ab258c208492 },
            .Y = { 0x2da903e3e17405, 0xb1840627cb2831, 0x95db47d688f83, 0x59394e75e34a8b, 0x5e8a6b911852bc, 0x1eb2f65172f301, 0x9d8315d7ce4472, 0xeb101f6d3f84b7 },
        },
        {
            .X = { 0x5f774f8e0de783, 0x18db342bd4f062, 0x15e174a1a34b03, 0x26467424d807f4, 0x5f3e56eb06685d, 0xa9dae27ea22df5, 0xc9ae3931f04373, 0x5606aa9b536ff0 },
            .Y = { 0x4f66a60a948308, 0x9e70add2b93a68, 0xef1ced675a34e6, 0x5f39deec1818f2, 0x81d90b22c7b013, 0xc799303ac6ae9b, 0x32e6681c950c45, 0xd68cd71e3712d9 },
        },
        {
            .X = { 0x25206038df18e3, 0xa33311e2c0e5a0, 0xbc7513c2427976, 0xe3cb24c9b1ea95, 0x6ee07fa274d7b7, 0xeaa6708f1d3e8a, 0xabbb3d99906c0e, 0x16a3d2b911b8ba },
            .Y = { 0x6e8ba140dfd96, 0x940d2a1d5d720e, 0x9821c84662e43d, 0x2b0439890a8550, 0x4247893cfaac91, 0x2a67671d936981, 0x64a9450f1bc1d4, 0xed01afd75cd4a2 },
        },
        {
            .X = { 0xf8dbf9e3702433, 0x3270baff4436d0, 0x886a254d8970d0, 0x634b73d79a2a1b, 0x224061a03967a0, 0x190bb2844f371, 0x20cb5c7c224a26, 0x8ccee5dec945d7 },
            .Y = { 0x3503723c337ed8, 0xea59bd2909e9c9, 0x370b18e9adf506, 0x4bd39c266a9b9f, 0xfbdbbd57c386f6, 0x6b5ce9a53bc757, 0xa09deb9965e667, 0xbd579771824799 },
        },
        {
            .X = { 0xcd27a8a4523ac0, 0x33223ec6461a2b, 0x35c189908f5bb6, 0x3ad91c3bef27e1, 0x5884e9b1cadaa2, 0xeb418026a6d67f, 0xcf86c2d47ca764, 0x2671ef7183530e },
            .Y = { 0xb4b3e01573efe7, 0xc74312cba0b461, 0x8a36c4ff1a7318, 0x6a0ce9af2c6292, 0x6180da982d4fa3, 0x1aa69bda78e038, 0x1f734fa34368a7, 0x9a56ef1f2e6aec },
        },
        {
            .X = { 0x22dc181ce00f5a, 0xfcab091b0b43c3, 0x8aa01d64b63f15, 0x2bce4511303e5f, 0x8cefed0144d7db, 0x75c35600766266, 0xc9b69792dcb39c, 0xeb8abde34d26e8 },
            .Y = { 0x4e6c7810ad587c, 0x845ee4ef7086f5, 0xa37ff00c774ed0, 0xba72717ec9c2d0, 0x14694a3a65ec4f, 0x141012156cee9f, 0x353c2f79170a80, 0x546666ab38cf64 },
        },
        {
            .X = { 0x177a79d28ac51, 0x434f22a9f95fe2, 0xc65ad9f6ac3a55, 0x4fdd0e24364ace, 0x3709785e697eec, 0xb92404843e715b, 0xe6b77436a68c86, 0xc043c3e49bed73 },
            .Y = { 0xe019d048ee6e0d, 0x55c257d624f3e5, 0x8bdc6f0b683611, 0xb69a1cf55819b5, 0x12463a0c0c0c66, 0x7195fe795e15d0, 0x33caaab309fff9, 0xc1c1fbd5dcc01f },
        },
    },
    {
        {
            .X = { 0x70e1871a27dded, 0x182ddbef1f72e8, 0x86301b15e1b072, 0xf27eeb317db93a, 0x4059e697671e85, 0xa84ec5dd788de6, 0x881f222761cea4, 0xfaa30c5595098b },
            .Y = { 0x788e7a5bbf15b8, 0xc7f0bbb1efc797, 0xc4d0c6d91e31db, 0x3addebebeff8e5, 0x961d078baa5298, 0xbfc9686dc54752, 0xb2a36a387d5918, 0xf6ba9fa0a39168 },
        },
        {
            .X = { 0x55d9fe39321307, 0x4282c744bedc62, 0xb5d861814669ae, 0xcc028e4c0d7153, 0xe1a8c82586ef2f, 0x83f93c5c7df59e, 0xe86cd756ce8f43, 0x7d7ec39c880adf },
            .Y = { 0xdc83c822b1a787, 0xc9503a5b95c897, 0x52237e5766bd44, 0x3cdb7017a64974, 0xfaddbfb7808aa0, 0xaa0a5b8138ab04, 0xf8575868d496fb, 0xf2b690e40d3b4d },
        },
        {
            .X = { 0x9f3ca13e9d060b, 0xdf622c2bfcddbb, 0x6a27bdf70aa2e, 0x2ec2e05f448762, 0x10308399098f09, 0x642adcacc9dc90, 0x34615bbec63652, 0x673e20fcf1e1bb },
            .Y = { 0xbff421935ae133, 0xd5cd1c24441f33, 0xece3c9f9f98df1, 0xefcbd49c763ced, 0x2ae33329f242c0, 0xb5a222219c736f, 0xf3577017c54535, 0x1a29efa4610929 },
        },
        {
            .X = { 0xe264d53e82d4e5, 0x582cf69f7e3dc8, 0xe426fdc78c50fa, 0x6914beef80d4a3, 0xab2a73b9d05c28, 0x8c57e85e8c0000, 0xc924e0e8fc63a0, 0xd576fe2cbf35d9 },
            .Y = { 0x968c2627b1f935, 0xfb6a2d07116041, 0xf55f6710892321, 0x9ae36dbc9d62a8, 0x9a3c745bf9c217, 0x62929205a83d15, 0x1adb66613fa7de, 0xa5f230cd7e5b4c },
        },
        {
            .X = { 0xe3a9764c55399b, 0xb18a8bac63a391, 0x506510287934cb, 0xe1c0ec5372132, 0xf17af373de2f62, 0xf43575d23a76e2, 0xeabc91cf7bbd34, 0x76fdc7bc58893a },
            .Y = { 0x15fa2251671866, 0xfb8d1aab057269, 0x42b168a0b7c93e, 0xf70548ef4fdb5, 0xd8897fc537c53b, 0x33fb9e3c052910, 0x97468d31950a24, 0x52589109f652b5 },
        },
        {
            .X = { 0xce2a16a8835b14, 0xbb05a910f3536f, 0x5b21882b361c89, 0xf3e593e2fc6c26, 0x5ca114f64fabb5, 0x41605127b494c1, 0x694a8b3327cef2, 0xd0b6b816b255e5 },
            .Y = { 0x1c9de020810d2, 0xa279cc2c8bad40, 0xfa26e7200d6ef0, 0xea2ed8158c1716, 0xc7d1be04606fc5, 0x9299f69d374d0e, 0x5e4d29c9a9a711, 0xa5c8a34504f114 },
        },
        {
            .X = { 0x815f7a811652b1, 0x1e311fff35eeff, 0xf0a65cd8b3d9b2, 0xcab04e0304bc31, 0xb40a5b5d8bc89e, 0xbeaa470809525d, 0xef0ae52c5c1723, 0x157c9bc39c0ac4 },
            .Y = { 0x5db2b61682e63c, 0xa1ecb74d987f5f, 0x3fbddd781f7826, 0xe4ebbad9eb8b24, 0xf4f48ca44b0697, 0xfb2d902fe96a78, 0xcdf8877f3d39fc, 0x84935bb49ba956 },
        },
        {
            .X = { 0x305cc6881da41c, 0xee8174b64eeb7d, 0x91ad3bd9e7a543, 0x8ec9d86022e9e6, 0xf32ac74f816515, 0x5aab0df035be63, 0xedbe9d907e081a, 0xb2ce3bde941bae },
            .Y = { 0x3a56b8a43d80f8, 0x2dc6dfb4a3e2e2, 0xe5de169950f0ae, 0x48493886a39985, 0x12e97c8f76b93b, 0xc5025e6e1f93c7, 0xf3340ae2e2e7ac, 0xbe3cfcaf9693eb },
        },
    },
    {
        {
            .X = { 0x654f6f63ad937d, 0x84bbf4ec553e3e, 0x4fb3f5eb70be0c, 0x1cc5b03e165490, 0xda5af5fe26a7fe, 0xb0fab3283c420a, 0x2eb1c1e112ae27, 0xbdb78d3f3e2b18 },
            .Y = { 0xc965f18e178c31, 0xbec93d48affd6a, 0xe691e4c9a18c12, 0x2476bb3208dd71, 0xf0a7ba5f818537, 0xcaec00c10d2b69, 0xbcf5c13910c731, 0x8e8fc8fb9fbafd },
        },
        {
            .X = { 0x1fa8c3e2a299b, 0xf4d582c13a9f0e, 0xed03719deda47, 0x7d517fd3c186c8, 0x202c0269b18975, 0xa2c9dbbcdaae47, 0x40954185f57f52, 0x20f66e40766f34 },
            .Y = { 0x49beef3e4858ea, 0x612b42eea22167, 0x4681ab63099a, 0xd16ed5bcdbbcf, 0x7261a30bc27b2c, 0x6792eb9a6e89f7, 0xf6156d1b75f05d, 0x5dc8709acbd65c },
        },
        {
            .X = { 0x5355d8c6ca9450, 0xacb51011cbf8f6, 0xe501ec628e48c7, 0x42c0d35360cb50, 0x6035d4d60549e5, 0xb22286aabf9c8d, 0x7a1acc2413b47f, 0x758fd9721aefa7 },
            .Y = { 0x5f1fda0a47076f, 0x8c5f61422baa03, 0x8c614efecf3abe, 0x59e8beb57adeef, 0x2a64014633ff11, 0x3c716d2d5de74, 0x77a058a3080365, 0x7db3bd3d7a21fe },
        },
        {
            .X = { 0x6f96cc6f990632, 0x843cbcb0d7a849, 0xdc4f7e7804554a, 0x4aba2d6b8d6b5c, 0x694a8799423392, 0x102104a2ebdb81, 0xb6ba1cbafd0cb9, 0x87384a2df31b80 },
            .Y = { 0x637d1e1806f91d, 0xc2145c06e549d0, 0xd6c8b797aba7e3, 0xa4f765f324d8d6, 0x2be1db157a7a4d, 0x361274b604431d, 0xb86a18c13ee493, 0x2e56e1393cfcc4 },
        },
        {
            .X = { 0x73dc99bfe24a1f, 0x3979cf30ae7130, 0x1a55ee8410c1a1, 0xecff62cf654e1f, 0xcf7860125cb2ea, 0x2515065c17a2e5, 0xf48947018a0eaa, 0x54135889d53f50 },
            .Y = { 0x7d0f511b06a89c, 0xa3b4a42eb98a65, 0xddc688d009f27f, 0x25f1acb381040e, 0xf109a9680b634e, 0x197741c250aa65, 0xa6756f27b7800e, 0x28daa9842de7d },
        },
        {
            .X = { 0x81b88c337d89b2, 0xce47ad7c748cd0, 0x2837576e5618d3, 0x8542e148924b73, 0x17a013f7cfe04b, 0x8d6e4518d48924, 0x6814d5a2d93c80, 0x626cb5cf4a6c51 },
            .Y = { 0x1ecb94b0a02032, 0xaf5066c0eebf30, 0xc51f5460d81f2a, 0xd720991abfb259, 0x4da0dcbe47418a, 0xb3b2a6736960a2, 0x5ea2e62e51287a, 0x89df74e5349d5c },
        },
        {
            .X = { 0x7051b8781e31ee, 0x2b75394a264c90, 0xe6081c92b8309d, 0x48366c3dc26e4d, 0x21b415617ee30d, 0x76349fe5d9dce6, 0x69ef71d087bc1e, 0xb2fa92acedb41b },
            .Y = { 0x7d03d0bdcba50a, 0xe6e0becc08a072, 0x79a6f930cd6ed2, 0xb91deee1e4cd61, 0x7c2b8a8852f14f, 0xf300a3300cb1a5, 0xe3ee1d4b68141e, 0xe463fec7d27fa3 },
        },
        {
            .X = { 0x1f26a03c84cd48, 0x95d567e8a5e585, 0xb29503fefa0850, 0xbd0036e0831ef4, 0x33fa4d1900d67f, 0x6f950e208c6d74, 0x7824cb27b8c4e, 0xcde7811151c4ac },
            .Y = { 0x2af3aa3d6f3a2c, 0x3db8a1b000c33, 0x1c63cc1f6756ca, 0xa4aef28c1d19b1, 0x96d3df0ecff5ef, 0x686feab739793e, 0x553b2cb7a5ca9a, 0xe377da1b69292f },
        },
    },
    {
        {
            .X = { 0xa40ce36dc4cdd3, 0x21cd5b3acb565d, 0xa3eb2cdf79a50c, 0x8d5b6805ec9754, 0xf81b92c4534ada, 0xc339226b921b97, 0x1a63676ea1bd84, 0x3879311dc52a41 },
            .Y = { 0xc479fd5c9da30f, 0x8b882d03bbec6f, 0xd8df7a93b037ac, 0xe3caaa6ebe79a7, 0x4fa8b5bdd91f89, 0x957412ed8cdaa0, 0x373e9ee3f83757, 0xa1077ba3f8d94e },
        },
        {
            .X = { 0xe31b24af93c851, 0xa193cfdb8cc9b1, 0x6a84e54b832643, 0x9c30043e4d5a95, 0xf3be39269b3cf0, 0xb01f67422c26d0, 0xb47c8cd4f7fc07, 0x94c725d53d5872 },
            .Y = { 0x98bbd8352caae0, 0xdbbd481981540e, 0x41edc403794573, 0x1f7ac7a87a9a09, 0xd5dfcd49740992, 0x5e20360e1fbb86, 0xef3810aa314ce8, 0x5532dc12f001e3 },
        },
        {
            .X = { 0xb4c0bb3a40a097, 0x20733da3c88643, 0x36046b9f90f145, 0x524fa44e8b7e10, 0xe43d4a9c961fc6, 0xdcb23527d9f317, 0x886bc1e930dc58, 0x5959b379490d8f },
            .Y = { 0x87acf2deb44881, 0x80d8953a0a18b8, 0xe62114dea34b44, 0x1853de3c34c847, 0x10f5c53b9b6c45, 0xea181901418437, 0xd5280301f5efaa, 0x3b10a2a54dea59 },
        },
        {
            .X = { 0x7118475575507c, 0xf35403de392405, 0xadc46d13efdd90, 0x2b1aa6dcb939d6, 0x5c2bbcaee90edc, 0xfafdab39d924e, 0x46336e38c4e2ec, 0x874db168ce2f8e },
            .Y = { 0x42de705195df2f, 0xd0895bb1e52d30, 0x6ebe15fa1a765e, 0x3c3ed4226b2ed3, 0x53491e31603cf9, 0xbab5a4e932433a, 0xf859ae1156066b, 0x8fa5f2bf184c99 },
        },
        {
            .X = { 0xeb6db3de2ea1f9, 0x4e783c8208ec34, 0x1cd6c1ada8115, 0x509259e64f4b6, 0x53fffdde6f269, 0x1f15fcee6a2585, 0x49f122834c289a, 0x3f8b4f9769aa3e },
            .Y = { 0x21a029f553a416, 0x9f3ed0c9e5fad0, 0xbb072826de98db, 0x12dabc8669e497, 0x4e79dafa43ec44, 0x60137dfb717425, 0xd3b334945b34f9, 0x78d506c053b4c },
        },
        {
            .X = { 0x4c60a7e85ae7c8, 0xd0bde75198605, 0xb61d9c0932452e, 0x698da458d68691, 0xc276261ae9ce3d, 0x7bc35888ea3bd6, 0xbc6d9d48a1d486, 0x64e1db30f30cf9 },
            .Y = { 0x4ce6f916407f53, 0xb7d617f171b195, 0x6f10ea82a869c7, 0xb4dee9efdfe45f, 0xa4add0817f8049, 0x4bbacb8f6fac45, 0xe8277a9001f3c, 0x20685274a32bac },
        },
        {
            .X = { 0x9d20a4342bd5af, 0xc22ff916008827, 0xf56394e4af66d8, 0x30c3245f6b4d59, 0x8c6ae97827744a, 0xfec00a6814a97, 0xcbcbd6b49c1120, 0xa472810656ed8d },
            .Y = { 0xfa7a19da308a55, 0x8a639d24030f03, 0x6c8c3506327a01, 0xb09c1f714a1571, 0xacb5d135e25377, 0x48e98f9324b35b, 0x9bf44abc84c19a, 0xafff7b0a9b798c },
        },
        {
            .X = { 0xaef7b9de7b0354, 0xade5dfad371621, 0xe655cac4edaa2a, 0xf6c8cfa107e91e, 0x237f65fabd876a, 0x111551b912ad4, 0xcfa4ad0ac0f365, 0xe5658470c935ee },
            .Y = { 0x73fc2d30d39e04, 0xf2d61ea1f2d7d, 0xb8e1c15d11126, 0x2cb99aab05ebd4, 0xcfb70ae5e251b2, 0xcf33e5fde2b4df, 0x171873fa3b258f, 0xa3da9617bab3b },
        },
    },
    {
        {
            .X = { 0xf0062c45efc7d1, 0xab47b7f862af8c, 0x2e492100c55017, 0xbf2aa5167d0423, 0x9ce02bdcf21f24, 0xcaa70353346c14, 0x8ca32b49828bbf, 0xcd7fc01da89b04 },
            .Y = { 0x1252ff4abcaa90, 0x851d2929862aeb, 0x8628218c8f669f, 0xac8329bf307d6d, 0xfdfaca44f8b5c7, 0x78ce15303acc56, 0x58d0b3db4e577b, 0x4b2e20fbc4acee },
        },
        {
            .X = { 0xc02aa0f5346d59, 0x73a8ec980fec1c, 0xe3292399af6af, 0x6be2391129716f, 0x2a0b0e0606e006, 0xd50b10f13ee1ee, 0x6f076cfcbab206, 0xd4c5a508b316b3 },
            .Y = { 0xa35f2f6f4e750e, 0xe8f675a1b6dae3, 0x66a6faa5b0a213, 0x8e05668f6b724c, 0x17c7ca82214c6d, 0x3e1a756c30c653, 0x98a008c051cd48, 0x8c390ed2eff381 },
        },
        {
            .X = { 0x6a6217d9a8a5f1, 0xea22facad721da, 0x7c429cb9ea24bf, 0x4adfed6581fbac, 0x8c937d83ecd818, 0xd0eba65754ad65, 0xda67889c909e1c, 0xa850284c81a0c2 },
            .Y = { 0x73b906aff28aa0, 0x49fc0f2087b59a, 0x815d5b3d9041ea, 0xb0fb76b98090d0, 0xe4ba7f5727abb8, 0xff3cfa3f7c206c, 0x11b03431967e87, 0xa29a93dffbbff2 },
        },
        {
            .X = { 0x28a522b289d830, 0x90ed7476f61f73, 0x76255439b1e1cd, 0xde2f4e8fcd2bf4, 0x94aeb485ac46cc, 0x2ae61cd8298865, 0xa02cac5cd60ab, 0x76770683e8dc70 },
            .Y = { 0xd84799e99f3500, 0xc74245022fd389, 0x89b635815c09f3, 0x16441a1d09578d, 0x7749a6812654d1, 0x9eca34fcc17a63, 0x93b9e85769ad77, 0x5c0dda89043fcd },
        },
        {
            .X = { 0xaf744f2a324de1, 0x79634f5c8d962d, 0x7fd074f66bbcae, 0xe8874ce0e0f4de, 0x3bb2846daeb5d4, 0xb938dc935e514d, 0x1ccaa2f07ec4e4, 0xf22ef6c48e4fcc },
            .Y = { 0xed19f69711982f, 0xa98f784f0d977e, 0x26592951ce75d, 0x60587a6247c844, 0x267fe744cc7c37, 0x8a098431a18d41, 0xde0b76b8b3e0f, 0x54f27ab3308319 },
        },
        {
            .X = { 0xdc81f44f4cbc82, 0x7763aadd11bcc2, 0x694c85c1e02ee8, 0xe06a3eb622ae15, 0xd030e488d10d23, 0x2ad6b01a18d4ae, 0x577928e523049, 0x178b8ccdecc382 },
            .Y = { 0x6a9a21e30b7fc5, 0xdf47ed606b9aef, 0x2b779383387185, 0x2adb07ae695b53, 0xf2e449bce5b9ee, 0x6036ae0917b560, 0x6d28b9743ad334, 0x3d64617fcce03f },
        },
        {
            .X = { 0xa9fea453ed599b, 0x7a5d3f651dee56, 0xdbabe68d595037, 0x5a11522f847aa6, 0xbee76a0b7745ca, 0xfa902fe80f10dc, 0x5615e5fb8f50c5, 0xea0ef1182499d1 },
            .Y = { 0x802e2e89822dac, 0x872e9737982df0, 0xb56e9e12cc6b19, 0xd94068ddcd2f2f, 0x8971e8824a730e, 0x68ab5b4c362232, 0xa33b4b047e270b, 0xb69c1f9df4b884 },
        },
        {
            .X = { 0x3a02640e03567a, 0xd1a9c3fb440bd3, 0x4a7718a0fdec33, 0xf206ad1e4b657b, 0x8b878a6bb989c5, 0x8d86c4c0f88f35, 0x2d61dcd3784cd7, 0x52dcd2dfaa123a },
            .Y = { 0xe62e7527952318, 0x3e7e20dede41eb, 0xd40943b011de9d, 0xe5bbddd3180ff4, 0x6e2c6a69382825, 0xfe3c044b718a9, 0xc8a2eadbc96aa5, 0x4e0ff92a325e4d },
        },
    },
    {
        {
            .X = { 0xddd63661fefe2, 0x26641aef9e5fb5, 0x4d89a1c7094516, 0xf7f2be049ecc54, 0x79096e74122884, 0x7657dc1725c4c4, 0x48ad557f97e386, 0xb6e76cbcdc5c2e },
            .Y = { 0xa8f454b7be46ae, 0x70f7215d7e4dbc, 0x6de8f3f775922c, 0xbb485f550bc34a, 0xadd649d77bdf21, 0x80476abed2829e, 0x7962787128a7a4, 0x23c161350f8a87 },
        },
        {
            .X = { 0x1c0c7ac5f201db, 0xf0d2e42b118cfc, 0xd55e1aaeb352c5, 0x90e5d2476ee065, 0x8c7dbcccf73fdf, 0x4d13e49b58f765, 0xb8c5cb84bfec52, 0xaaaf7705853522 },
            .Y = { 0x4f9899ca66c1a5, 0x4123888c33f570, 0x3e657edbc087b7, 0x3a587a39212351, 0x6e20aff03a0240, 0x3e1dbe9d369c4f, 0xc6ccfcb1b91769, 0x6ff607621adb70 },
        },
        {
            .X = { 0x376822e78195e3, 0x7320b4d38cf2a4, 0x7d4f170a131882, 0xb4909d4bd74c1e, 0x8e59bc0a8a1853, 0x2b5087d6f481f, 0x5d4d24b1cc408c, 0x20507240418074 },
            .Y = { 0x823d49d02af51b, 0x8d23b4fe3230f8, 0x71dd8fc6ff6e7d, 0x7fdb66fac35838, 0x54565261cb59f8, 0x8dbf33d68dbba1, 0xafea90ccd60ec9, 0xd5ba6d78937d47 },
        },
        {
            .X = { 0x1909a5567289b9, 0xca7e31b36d6b5b, 0x670e704deffc2f, 0xdca41acec9753d, 0xb55166fed33067, 0x6a32153ecd6d0d, 0x3d5091c7d3ea3a, 0xaee6d0aaf08daf },
            .Y = { 0x86bc7308130788, 0xbae01e9757c311, 0x96faa2410c9ff1, 0x40ba1ea1db73fd, 0xf8db945c3ea22, 0xe4c62e0024446c, 0xdb27518bd7dc87, 0x40ce77bd2fc7a7 },
        },
        {
            .X = { 0xed20c8c4b5ba2e, 0xcc8f59812a3ae, 0xa2b44baf3d47ee, 0x9632815f1e3e9c, 0xda906acacd5fb0, 0x1b291ef3b60b72, 0xbb8e80034381ec, 0xbe9d29092db66e },
            .Y = { 0x878f2f75dd1052, 0x11e2fa1f653ec3, 0x747f8354f16044, 0xa49c2df3b70191, 0x740fb9d564f02c, 0xe1f1915a8ac685, 0x2c94b8751372b3, 0x50f0ef3d123439 },
        },
        {
            .X = { 0xa11fc884eac7bd, 0x42acfa7f899e9f, 0x721e684a3b5fbb, 0x330cafc9ac7d24, 0x8fa361fcf84d3, 0x588e8c1755aa3c, 0x6fb8cde5f0e731, 0x29c3b6ac2069a5 },
            .Y = { 0x3bff2001209e3a, 0x813fe3eb21d91b, 0xbc25f396556534, 0x1fe184bfe20785, 0x506bed63a8449, 0x62421176b5e0ef, 0xe97247f0fa4b21, 0x668868600caef7 },
        },
        {
            .X = { 0x2617b441b825ee, 0x6f710c07c5dbbd, 0x97903ab453e9c6, 0xb42fd590cdaa38, 0x9fc8a09649b1ad, 0x6d373267fe7018, 0x8c74d2b64d260e, 0x1e97ce3d283941 },
            .Y = { 0x772c76d7ea35b9, 0x69387f8c099676, 0xb4730bc9a45786, 0x905c658dfddfa0, 0x318fc3952269ba, 0x6c7372d7a0352d, 0x9a6f8903b133f0, 0x4f1993700a18e2 },
        },
        {
            .X = { 0xd34f5ff6c8585a, 0x1b21921fdc8ec8, 0x3081f8b07007ad, 0xa577e88ad9733c, 0x30fee5b9d40b79, 0xdf1b76e6ecbcb8, 0x4c1320b2040788, 0xb2a1a29549d284 },
            .Y = { 0xc2d70f11fc409c, 0x1fcc0c32f4ea0, 0xa7d930fa9ddc7f, 0x1c6a502d47d124, 0xc4fd7941601dea, 0xd66a36931a16ae, 0x2a395526bf3cbf, 0x1f2adc6de63642 },
        },
    },
    {
        {
            .X = { 0x46aedc773c4986, 0x2ae96a22ffcdac, 0xbd5cf608d88ded, 0xa60b95bdf2e595, 0x8a6e1e06c6fa0b, 0xa8c362b8e2e3d6, 0x1db7c1dd141c33, 0x51f65cdc14437 },
            .Y = { 0xf817791a9d5e19, 0xfc8cdb3a66aff3, 0xc711380065b7ad, 0x90b5c13fd5b43f, 0x69b7c5f278d6ba, 0x3f15a08c416010, 0x5a7eb995508e4, 0xd5c0ebcd813fc9 },
        },
        {
            .X = { 0x36488dc13285bf, 0x1f98569c904d13, 0x12b5ef55c9a530, 0xb7b1e95f62be6b, 0xe6208b49f3c3c2, 0xc4940c02faedb6, 0xb5268f34913e6c, 0xd8e9f3f3828d75 },
            .Y = { 0xe4aabe61d2f467, 0x62868a56caaf3, 0xf306ad1d117146, 0x634b8e83933cba, 0x3bdb1542f3385a, 0x74d850fddc272e, 0x5245bb81d7728a, 0x251b8878e95cb5 },
        },
        {
            .X = { 0x2b1a54565640af, 0xcece7a3ac042df, 0xf3f4d8539edfc8, 0x9d94820dbd8d8c, 0x9424c9f114b95f, 0xe4556554b43a24, 0xd5bbeab62df9e8, 0xe7859bd37a7b6c },
            .Y = { 0x4c176dc897d071, 0x1984f525882b20, 0x191e245e353c1b, 0x6852ff68a34e59, 0xf6f7311990c8bb, 0xcd6ab9837a3b70, 0x49554695dc2890, 0x6e728aaa485abd },
        },
        {
            .X = { 0x53aec56d2da59d, 0xf080fcc7c6d6cb, 0xabd986cd88bdf3, 0xd4139b0b7ad08f, 0xd70ef5b58ccc95, 0xebebfd56b4ab19, 0x3a4bd96b72a94b, 0xe52202f6f6fd5a },
            .Y = { 0xa2121f004a572b, 0xf904a70f66e6b0, 0x5c0400a1b730a2, 0xba29e30931389c, 0xfd2b3cfda04f5a, 0x18d14501c28851, 0x57548e168295da, 0x85b0aeb397dd46 },
        },
        {
            .X = { 0x295173e86f32aa, 0x4baa5a1c5e7a96, 0x570d29f6749ed7, 0x582764947e8513, 0xc3834d44215265, 0xd984eea9f0b660, 0x6c1af49e018578, 0x2ef2dfd525b3c7 },
            .Y = { 0xa9247e1b60f21c, 0xfb823db01e37c, 0x8409056b9b97d4, 0xfc095302a4b6a0, 0xc627b0dd35009e, 0x93c7b1916ba0f5, 0x8b84deec195b13, 0x37351d98efcb80 },
        },
        {
            .X = { 0x4977a685ce24fd, 0xa778016f42f9ee, 0x2723c5f77a59db, 0x146d4a07df81b2, 0x7597099b8e5a00, 0xd4e30e0538e854, 0x934762bb146cea, 0xca2518eedb1838 },
            .Y = { 0x1ae3fe7bd0e784, 0x179a33a217996d, 0x835001265a0f25, 0xfe06b450e90b45, 0x7e044ced0d8e63, 0xf127957a4c6fbf, 0x402a8e3678f53e, 0xd6c5752f4b967e },
        },
        {
            .X = { 0xb63239bd43fe28, 0xc5cdac5d776431, 0xa295dca162ca7c, 0x21c946ef55706f, 0xd35fdf1ed9d0aa, 0x981a8e7f42ce01, 0x988a85f4598ee, 0x1765bc88281f53 },
            .Y = { 0x5e402ec6b1e3b0, 0x4b34781ad36174, 0x5aa203b6fced30, 0x64c210e6157f1f, 0xd3a82225080059, 0x27e627ecc11056, 0xf33c8141667b46, 0x7736ff77fb96f6 },
        },
        {
            .X = { 0x7c61f3ccf1b795, 0x403b62f897de75, 0xe941dece12aa7, 0xaa8ebee91c38d, 0xe1cc1823c5b7ab, 0xdf664abaddd6c9, 0x7fa93dbfbe764a, 0xd49137d7a88bb8 },
            .Y = { 0xafe16b383949ef, 0x729adf0f6d4017, 0xe5fac6152ecc05, 0x8e716a047695d8, 0x2dada7da8b44f8, 0x85fa4588b225d3, 0x2b23f8363ef58c, 0x3d0e6ccda92e89 },
        },
    },
    {
        {
            .X = { 0x68379e8b334c89, 0x8cb447d27033e8, 0xbdb2993713cc0f, 0x1a367e7ed0d518, 0x1c399dabd4fd28, 0x99f6c0e9342786, 0xa8edcfa4a35c10, 0xd4861ed1b5d6a1 },
            .Y = { 0x447c236985cc05, 0x7cb4b63c3f52cf, 0x6ac0f8672ca19b, 0xfd0c2510d8a27b, 0x612e6d3c16a80b, 0xb20ab1dfb86104, 0x4bdfa306328191, 0x4539e9d1469099 },
        },
        {
            .X = { 0x10c6fde62d40d2, 0x33b01ca4ce415b, 0x358d3eea83235d, 0xe07664a36ff54, 0x4e1cf18a56185, 0x4f7f46669b13c7, 0xeb0e2637421409, 0x4079e7fc08f0a2 },
            .Y = { 0x911c01e33ee9ab, 0x8c1d1cf17b9f2c, 0x3c40ce9d18369, 0x6df14e4c7f5fe8, 0xf16c4d091498c4, 0x828b0aa9468811, 0xf657a8692d47e8, 0x6209b828b07635 },
        },
        {
            .X = { 0x69518500a6bfe8, 0xe8f38fee591d4e, 0xda2e0a8d260cc3, 0x75983b20b47288, 0xa8264932172b18, 0x31fc31dd756095, 0x61e78d08d2eeb8, 0x4479493ae95fa9 },
            .Y = { 0xb7a7c06f16c19f, 0x80e94b897c7f4c, 0xb5b2f90a9b7efd, 0x4ec0a3c05f50c8, 0x5d5cc927c4887f, 0xf24ab1bf2e900a, 0xf18ad153298ad5, 0x9d2b81f2c2922d },
        },
        {
            .X = { 0x88916f30696af2, 0x1c49a5576fcc11, 0x49319e4609f8fe, 0x6ef274c929646b, 0xe2a3f730744c13, 0xb22359c96fb1b6, 0xb7820f48af2133, 0x46de62e61d7611 },
            .Y = { 0x4a202793bd3c49, 0x4602ebb93f3820, 0x376f581c499c7b, 0x5115c6e6454550, 0x9ba1b1e81e7ea3, 0x48016e184bcee1, 0x4ea6453462645e, 0x12d1d9a80858c9 },
        },
        {
            .X = { 0xe1455829f3c06f, 0x24337a9f920829, 0x4ff5f216e828ee, 0xc91e35211714dc, 0x8b8c92bfa97043, 0x5bf7b30460c5d4, 0x8866d5b2545002, 0x9def853cf08405 },
            .Y = { 0xaeae8bc5628028, 0xe26cc945d061d6, 0xc1e5008186dd83, 0xc06c366e1b1489, 0x8b12914e0e191c, 0xfaf58ef00c06a5, 0x5080eac50fc51b, 0xeca0c70c398292 },
        },
        {
            .X = { 0xe9568c6ee1d558, 0x4cafc8949a3488, 0x4b4e5c762ef55b, 0xc83d0530020264, 0xced7cb2c12d20e, 0xf0813783c24632, 0x931585eb76967f, 0x2ec27ca844a48b },
            .Y = { 0x3da20db90210d0, 0x27ae5b6e6b3c1e, 0x59f568007fa3b4, 0xc70f65f6da6562, 0xf85a00f0ea8a6f, 0x695326d2fc4297, 0x85fb070d349e10, 0x48c42796a221d5 },
        },
        {
            .X = { 0x341831059fe520, 0xc5e07921bcbb21, 0x4c94946f057376, 0xb462752c5af856, 0x316e5f06d3be70, 0xdfd0bf9ec9dd75, 0xdde0988ac9b082, 0x16c085975f60b8 },
            .Y = { 0x8998e4c4a6dcc0, 0x6ee04f3453f160, 0xdf87b70cacf63c, 0x3c656743cf0cc9, 0xc3a4b4d786342b, 0xad19761b5c5789, 0x90947822d60215, 0x4570dac9ae5fc1 },
        },
        {
            .X = { 0x1a35c1233815ec, 0xaa6b8528495da1, 0x8c23e31c5af58b, 0x1fa5e35acd0141, 0x3abcef12970635, 0xaa347b73806530, 0x7d031c2a40b0bb, 0x454e4d8e3f8156 },
            .Y = { 0xa29b7f96a020ee, 0xa8957f95ce84ed, 0x540b9709159365, 0xdb46b8c7d6e3e9, 0x61a1779d38dfed, 0xb3361fb56c7bb7, 0xf068450cfe0597, 0x9543d7d9afd575 },
        },
    },
    {
        {
            .X = { 0x322844cee52403, 0x622b406642b340, 0x426eaabaf11bee, 0xef16429d22b0d8, 0x33d181f6543504, 0x99f717747e187f, 0x7e632286ea585e, 0x86e6b98d35d79d },
            .Y = { 0xde491ff2867761, 0xd2fba1e100dddd, 0xf8eec31275d44c, 0xbd463124e7a80a, 0x4592f89b846878, 0x894f15a355d5fa, 0x84b3e182d3d525, 0x76cef8d9ff18f },
        },
        {
            .X = { 0xf484b5485078cd, 0xc46591458d42c4, 0x8ce79c608d01be, 0x326150e56df37, 0xfce040bfb63d8e, 0xc697f8ce9b33d1, 0x55b9fc3cfd8bae, 0xe3658a8991f9b6 },
            .Y = { 0x1e26a130698841, 0xe2dc58f8c6df8a, 0xa7e3943fac394c, 0x796c492d298c66, 0xd9e06854465fbe, 0xdc45236c451e25, 0x1f3ecd0b70b629, 0x6d4e1af6f03f53 },
        },
        {
            .X = { 0x52a7f64aab551f, 0x541a7f626174ff, 0xd16ab7b0b1387d, 0xcc9a9b7d9027e3, 0x144adced0ffcff, 0x83df8899078bd1, 0x1308caaad8ad2c, 0x5e5b75783c1a76 },
            .Y = { 0x23959c867f4ebe, 0xd505e46c6ab4e5, 0x7068ad439d69d, 0x528312c21bb5bf, 0xb81d04a7dadb10, 0x4d18ac05b33d8b, 0x7b87f8501a3976, 0xa986897bff2bfb },
        },
        {
            .X = { 0x6a21b5c699f096, 0x8f63d921f4ea8e, 0xc58017fdde7a93, 0x1d4a6118d8f5a0, 0xb3620a68b1d0e2, 0xc045cde809e072, 0xea227a611fbb77, 0x35da5cd71cf953 },
            .Y = { 0x6af675cdae5410, 0xdf134ffa97888a, 0xf44b5268f73560, 0x622295f98aa177, 0xa24a5401bcacc4, 0xf42a937645585, 0x39438b22d01df7, 0x6bcc323f4c228e },
        },
        {
            .X = { 0xbeb7379a9565f0, 0xad5a3ecf665e14, 0x7a0ca798228194, 0x4b956fa22cff99, 0xfbcea6080c0dc4, 0x6f39917d5119ff, 0x680c341230e413, 0xbea005b9db529 },
            .Y = { 0xe6bc8299ee0c63, 0xd8f53ac2a137aa, 0xa32c8cd7280a8f, 0xb512f91e902036, 0x3b1802ab553172, 0x451e78d636d095, 0x35bd79ff132a1c, 0x699a51ca6e3685 },
        },
        {
            .X = { 0x8e3dfdd4959827, 0x39ba8e44120b8b, 0x4f7bce2e8f52b3, 0x96b53541c5b4f, 0xa8e008420ba4a9, 0x4e8eaa5553dac1, 0xeb9e60d4a98cc8, 0xec9eddd33cc64e },
            .Y = { 0x54c4b812f285ab, 0x8dbee5b5dcdc1d, 0x15c0fe4529330e, 0x59168b4de8e12a, 0xf35e4dcb20c834, 0xb9052e97bc6517, 0x3549626ee1c935, 0x4c45b81d139c2b },
        },
        {
            .X = { 0x50a8fd235bb7ea, 0xa8d1dac5efd877, 0xf80821a23b4a8d, 0x9b6e962d01cf8d, 0x77ca6811823bd2, 0xaed0cf74aa532c, 0xca95d36b44c965, 0x5f614a88ab35f5 },
            .Y = { 0xd62909b6e22080, 0xdad125a69ee59b, 0xf201d35cf19986, 0xd4376b93edb6e4, 0xbcf2006529b0a7, 0x2921b33f410d00, 0x14d6020bbf79e4, 0x4ba1be5470bc3d },
        },
        {
            .X = { 0xf3701e5299f83, 0xef0111ea0dac41, 0x95888dfe32a166, 0xc13b2017fe5beb, 0xb763af45eb1b9d, 0x72bc0d46edf62b, 0x960b0e22b4613a, 0x1da2dd7ccd398 },
            .Y = { 0x78c5ac38408985, 0x3735e7b521f2f0, 0xe0f78738d64331, 0xf5c6b6b118bdfb, 0xdf17e3a60b2998, 0x925ced2d9166b5, 0xe9eb98d0d100ee, 0x9b05404fcb7b81 },
        },
    },
    {
        {
            .X = { 0xcec48d519a7b4d, 0x9ea0962db23eff, 0x53ee29de4f4f86, 0xfad1db3f75a5f5, 0x94dd2ab6cf992, 0x82e0bf1facd69d, 0xfa54416ef4c120, 0x80b4c24ed84338 },
            .Y = { 0xbd917015638059, 0x9a275665d22e5e, 0xfd53d1478f6e39, 0x20a730218d6f59, 0x4ac14759f2079e, 0xe2d0f4789470be, 0x85797ddc80209b, 0xefc0475c82e0a8 },
        },
        {
            .X = { 0xbe51fe7e54d3d2, 0xc2366028d8c98f, 0x5e8a8c1b09c1d7, 0xce6d60c14b11b7, 0xbd5df349b54cc4, 0xe6d11cea361835, 0xbd2291313e462c, 0x8521b7d6a1bd45 },
            .Y = { 0x8d4cb8c4a0e35c, 0x97fed84b5cc640, 0x71c04a516c8e7, 0xf9549d103115ae, 0xceef85424a50d5, 0x7b941a6f800dcf, 0xa9b335356004e3, 0x88de37352257d6 },
        },
        {
            .X = { 0x7c669f53af0536, 0x6e299da840b1fe, 0xaec534e3f1f173, 0x5303252b21976e, 0xb91d7fe6a423a8, 0x141f25a325195d, 0x27a8357e28e3af, 0x148ab0c0f245a3 },
            .Y = { 0xd3f495eec74b40, 0xa133595cf3da19, 0x74112c977208d1, 0x934f3fe56df6c4, 0xd6a07d1f5f2ad4, 0xed4d67e24066d3, 0x8a11b854de42d7, 0x99d67e5b1d1856 },
        },
        {
            .X = { 0xa629b81f4cd33a, 0xb5c3119bfc3f9, 0xd66c501004797f, 0x1d5f27c6d16329, 0xf8dae62012bef0, 0x49b486658874b, 0xf10d0c1b2f4c27, 0x7c17759fabd71f },
            .Y = { 0x72077ab5c43e47, 0x6c1c65354603a3, 0x76740727f81634, 0xdd525032c28710, 0x3cc563a832ccdd, 0x29e5e4c310d597, 0x59c84b0eb40640, 0x299793f4459f2d },
        },
        {
            .X = { 0x326d57813a1a4d, 0x8314fbfc60e88b, 0xef5c1a320f832e, 0x38996bf27603cb, 0x805dc599aa85ae, 0x7c5185948b742f, 0x53daec6777a9fc, 0xb6769121e09d7d },
            .Y = { 0x473599316702ac, 0x506d5704b3f67a, 0x6435e5a10623bd, 0xb35b6e35cb70fc, 0x5841caf7a758ec, 0x64b99e78e47eb, 0x20c8168ac2bd76, 0x76c32f727f2766 },
        },
        {
            .X = { 0x8c85ba181810d, 0x6852162d17c90d, 0x63f934dd485f67, 0x2686073295c0b0, 0xad02168ab4a9a, 0xf3e522cdce4420, 0xd9f5c876b09a10, 0xbdc8824cfdf3b9 },
            .Y = { 0x5db60202376976, 0x82b57127b7fc43, 0x8bffb9237249a3, 0x56cb37cef5b308, 0x6bb8882fdedbf1, 0x403e29866f650a, 0xf9b5828591043a, 0x7d94883555b05 },
        },
        {
            .X = { 0x9694a6c77339, 0x9e58b3d15ac79e, 0x5c45336a5aa7ca, 0xd38d154dd8783c, 0x6bd3898359d167, 0xa79cba4c3df68, 0xe5bdad3005f930, 0x148c2ca042cbfc },
            .Y = { 0xda96d88e1c78f9, 0x1b86a3241bb2cf, 0xe406d063d96cc, 0xd336e1179dc7c9, 0x4c37f91cf11561, 0x62ffb969d9f573, 0xf4366275af84dd, 0xa11e473e5b475b },
        },
        {
            .X = { 0x3666085f3de3bb, 0x7502ec1778de8b, 0xa715926ffa2b65, 0xfcb083d228c163, 0x37dd87e54a4a9e, 0xe64859e39df8b, 0x681e6f25018bed, 0x3b0b93c630797f },
            .Y = { 0x19661e2487aae4, 0x4092c4a0eead6f, 0xa81bcd189d1141, 0xcc399987ad23da, 0x4d86328b8b8a23, 0x38435d11e9b41b, 0x7ae9b111844628, 0x7fa254231402eb },
        },
    },
    {
        {
            .X = { 0xb4de94d177cfa, 0x13db6ca0584aec, 0x4c4a7d35384b07, 0x2bf2f2ad8a8efb, 0xd773fd3ee02801, 0x74abb0b78630, 0x967d5b079a453, 0x4b1ce7b6cd3303 },
            .Y = { 0x38a4b6d4b7c608, 0xb76fd81fb548d5, 0xefb09e09c6db17, 0xb81a677ca00192, 0x976cd5db5bb084, 0xe1ea8b503caea9, 0x6892ed8ae1a8bf, 0xccd3cd7033bbe6 },
        },
        {
            .X = { 0x1eccc95c7cee64, 0x711fba3a0971ce, 0x6bdee5bba0bf81, 0x9e384de8fe28e3, 0xe610341902404a, 0x218d5de89acbf1, 0xb2bb94a046b6e2, 0xfb3380967bd379 },
            .Y = { 0x3013c0d87d4b87, 0xbe51ab7da883cc, 0x4d5d232bcc2525, 0x90f360dd483547, 0x489a386b492e4e, 0xe858d090f6f030, 0x7da630f08cd9c2, 0x522d23bc34af0d },
        },
        {
            .X = { 0x527f3990afdc6d, 0xc4f3db596e721e, 0xe1bad9647e86d2, 0xd17fd6942a3955, 0x59dae9a65eec2e, 0x491e01445defc8, 0x2475deb23070b7, 0x56715432282501 },
            .Y = { 0x6926ec725c80c2, 0x23cf0610240c86, 0x40072c0f8fcd60, 0x9533e0cbfe73c8, 0x9859198b4f40ba, 0x71ad68b161bc1a, 0xbad9472e9dd4b2, 0x2812e1e3516e92 },
        },
        {
            .X = { 0x8199dddb982ba3, 0xa8eeaa68a695f4, 0xd299e6883576f0, 0x822e63ff206f6, 0x8477bff7fd52d7, 0xc6e7e2f6b21a4b, 0x1e57df10807026, 0x69c18c24a2a715 },
            .Y = { 0x42f92579c6ec94, 0x94be18e6c81a8c, 0x2fb9421f648c05, 0x13a3e955bee25b, 0x186e4ec1aa7001, 0x641264af8c4601, 0x89b61a42c57ce5, 0x57479487cda6dc },
        },
        {
            .X = { 0x7f4f3677c30e2b, 0x381977fef6ed59, 0x92a64345e977d5, 0x3849db1b488a88, 0xb3fef4c8983f44, 0x175af25695dbfe, 0x1a81f28bab06ac, 0x28ab9ffb49fab9 },
            .Y = { 0xa58efb5ff05d1b, 0xd2388a79f7db71, 0x233cac18936d13, 0xfec22b9f41a823, 0xcaa3169d332333, 0x2b4f878211ad66, 0x2b63e2c3ef7f58, 0x3f93812787fd25 },
        },
        {
            .X = { 0xacdfc29c490f03, 0x6da19af3a43099, 0x16232f4c07b498, 0x4026756318ecd4, 0xb0d416ff63e3cf, 0x6de8eb415ffb8a, 0x6952e05ac32b5a, 0x999a6cf9be8ca6 },
            .Y = { 0x268b7e0b4d710c, 0x1fbbff13ca98d7, 0x9988e42de1642c, 0x2187d324772994, 0x9a624fff5bda2b, 0xc5eaa5d05cd1, 0xe8c7fb1e257cdf, 0x8e6d4379aac9b8 },
        },
        {
            .X = { 0x5127a85e4b6b53, 0x5e06627fada53b, 0xeec06621300435, 0xde84087e0a6b2e, 0xbfccb9b2455273, 0x1c3308198be253, 0xa89031e9f47ff6, 0x93d13e11b061d2 },
            .Y = { 0x30de7e60590f1e, 0x32f76138c77823, 0xbdc20fdc740ac7, 0xd1645cb906538b, 0x9804e31deea40a, 0xd228548e40a836, 0x18e6423ab71415, 0x7c7ee9190bb0e4 },
        },
        {
            .X = { 0x9ee0b735d8c8ce, 0xd1817b49ed3c9e, 0x416d45775599c4, 0xeada3679eb9dfa, 0xdec17478704b1b, 0x88523143638810, 0x49963d2a2d8e13, 0xc57412138c0c64 },
            .Y = { 0xe6e3724588c8f8, 0x51603d66a4971a, 0xa090bd66ef6c2d, 0xc9d2d31662f9c5, 0x193c781dcdcf57, 0xa8221af3983a06, 0x579f50ce196ce4, 0xbfcb28add9cc6d },
        },
    },
    {
        {
            .X = { 0xa59b3ef6b50445, 0xb76d32307c0a62, 0xfd9c2e1737864, 0x2320bf99689b5d, 0xb585618d355994, 0x7a89d2f1c723ab, 0x24aa03f3cc0245, 0xc127e36b81f0f1 },
            .Y = { 0x87949324fb5df2, 0xb6bfbe6beb22ea, 0x57320e5fc92b36, 0xede6af0c9bc205, 0x8b55d6401ccffd, 0xe37702667b00e5, 0xf89ac3994f6732, 0x6f07bc1b02f573 },
        },
        {
            .X = { 0xca9686de62a980, 0x1bb0b626b2cc0d, 0x523bd8995a487c, 0x7a20a7ab09f393, 0x5c185ff26bde24, 0x4e272b1c603928, 0xc496e28a3ec41c, 0x42042d5b170ab6 },
            .Y = { 0xe5752846fbb5be, 0x67bbb2534c2eee, 0x42daaee3d1b7fc, 0xccd93bd682c904, 0xb4c1d61d7d913c, 0x72fa3bdca4b9a3, 0x2c3516a5d8fb02, 0xabd096b98498e4 },
        },
        {
            .X = { 0x2421dac9ea91d8, 0x910ad3d3306689, 0x3c7bd25096a036, 0x17bd0547c7776c, 0xd63b40ff35b85, 0x9a79b2a6e5c736, 0x17c49ad31f0345, 0x5b73832741c083 },
            .Y = { 0x359375f12b99b2, 0x9248c49a6ed917, 0x5bbd8632a1c1d7, 0xc0e96838f155db, 0x3fdff0b21c4714, 0xc760e3a31b865b, 0x84dbdce2260149, 0x998d2310bc9765 },
        },
        {
            .X = { 0xf187beafe75b26, 0x2de2790d3d9f4f, 0xcbf49669f76349, 0x294e6917a4fc50, 0x7a65166a3fb0f6, 0x721ae8025a94cf, 0xb425b118bdd4b9, 0x25d76431c49703 },
            .Y = { 0x4cabda3f0bdafa, 0xc41b3261c2b729, 0xf814647910f0bb, 0x30aaf77e4449a3, 0x94b700272aefd5, 0x3d509e4e5a5f0b, 0xebc51c2570d22f, 0x2ae33427582f32 },
        },
        {
            .X = { 0xd904246922c35c, 0x76631bc9caaed2, 0x5756cd9626fe77, 0xf08ca9841b575b, 0xf41ef32d7f1628, 0x2cb8e131c576bf, 0x3cc6fcaaca322, 0xf6afb3d9c2ed5a },
            .Y = { 0xb68a3920da080e, 0xc9d474a9e957af, 0x1e6c7d82c3b39a, 0xb69ccd2b6e20a, 0xdfa8789d9d71a6, 0xf9c3c7b4e4e66e, 0x69e3d3a3aed4a6, 0xc363eb29e12a4c },
        },
        {
            .X = { 0xcdc639944b0c95, 0xdaef084825ab7e, 0x1c37473302bdf4, 0x1d25ff697531b1, 0x3cae3b5497b6e4, 0x9197d751418f5a, 0x3c5489ed1640c8, 0x140bebeb88338d },
            .Y = { 0xc9058fe4bcc0ff, 0x206944f5ae266c, 0xd0eabc94970774, 0x9c108c9bfc3e00, 0xf056b389383de4, 0x56ebb6e717265d, 0xe1871669eda328, 0x974017e289a5e2 },
        },
        {
            .X = { 0x69d675ab5b0d9e, 0x3f67bd0eb50763, 0x21c1fb4f906be1, 0x489686d2295fbb, 0x52d51b1c493f90, 0xfdedd0c2ee49b5, 0x7c5ae8c0a5f597, 0x1c37cc7def3280 },
            .Y = { 0xfa1703d4c2f45d, 0xabc10e339a78e8, 0x8b2ac8e8e511fe, 0x9978d5519e2a89, 0xd2209f6a95124d, 0x4d5fd879e99a0c, 0x61aa20b5c765b5, 0x153bac44a8676 },
        },
        {
            .X = { 0x23916a7f3f04e7, 0xcad563616a4a0, 0x9872ad81614f0d, 0x7acf757274a5fb, 0x9695c0ed7951b4, 0xe092dce17bf8a4, 0xbc7773ba808ab4, 0x3466868a7b8425 },
            .Y = { 0x100b9e26a5e59d, 0xbb0bec874d4145, 0xdaa8808a4015f1, 0xfe9e9c2c262545, 0xfdf7c8beac00b, 0x4619f2ce330211, 0xf10488fcce7f17, 0xa329c5e3aebbe5 },
        },
    },
    {
        {
            .X = { 0x7739f91a2606a2, 0x4a98340b7a32dc, 0x11bc57f396d1f6, 0xe166adeebd1117, 0x5607a3ec1625a6, 0x808c76a19fe6ef, 0x6401666e12d3b6, 0xb2eaca15c2bb2e },
            .Y = { 0xa24e6c4a3ec1a1, 0x25e999ccc55681, 0xe4f39ac7c9798, 0xb959924a44bc1d, 0x21985d7bd984e2, 0xcdc6220e4d2ece, 0xfa37343c08f65d, 0xbf22bf912d442e },
        },
        {
            .X = { 0xf178ee3287f4b3, 0x66f1a3dc47cdb9, 0x80d556d12be608, 0x35d81e82f9558d, 0x40aa3bf8c3241d, 0x8cf199fe8b6deb, 0x9e06ea86182250, 0x9843992beec953 },
            .Y = { 0x1a7fcb49224461, 0x37fb9d454a34c1, 0x231a6318a1268d, 0xea9343af9e8a20, 0x70d06a482a5002, 0xd2352e2ac05ff5, 0x38360e6dc6972f, 0xf3ca154abcc098 },
        },
        {
            .X = { 0x8effe12bd49a0d, 0x42cb0a035939ec, 0x2e0acdf1663326, 0xdffdb5947a4465, 0x357ac2a03ad5df, 0xe1bae559092719, 0xa53405418cb9ab, 0x1dcc89783f737d },
            .Y = { 0xab8e10966c87bf, 0xca1e844bb21710, 0x13c75c100634ee, 0x4547e5155aa701, 0xdf3248621f2140, 0xe3ec8fa1695211, 0x10dffa05f12ab, 0xceffd9e4338eef },
        },
        {
            .X = { 0xaffc6a96ce6f05, 0xf224730c933b89, 0x5f5991e8c5fc09, 0x1349bbb10d6622, 0x604fca5a13b9b6, 0xd0642ba4f9b5e8, 0x7ece6e8481078f, 0x81c164a809f9bc },
            .Y = { 0xb348c78edf5f41, 0xade9cd48dbc144, 0x4216744ea2f7fb, 0x2384d4a8bf133d, 0xff60e1c98a172e, 0xfc3df3ac016cf, 0xaaec44135ac5c8, 0x74915f7dea9aad },
        },
        {
            .X = { 0xd7f9c884ec3b7e, 0xf4a7e5553a1916, 0x4bbf83529faf9c, 0xa05a3596485321, 0xb5c31454809de7, 0x8ff4ec6dcd3f3f, 0xe5538480f389, 0x1159f63e029c61 },
            .Y = { 0x6e49dca3e22a90, 0x98cf58f2cc009e, 0xc280c946973e9e, 0xb6762c4fa37bfa, 0x6f4f3b2b4a89df, 0xa47b3fbc099e01, 0x4b993912e325bd, 0x14afa9003b4b8c },
        },
        {
            .X = { 0x7190d38cb61d30, 0x44aeadb7f418fd, 0x1c49f643f051f6, 0x11a129ecc16f60, 0xf4a15e66538587, 0xb866aa1566bba5, 0x182b8e4eee5ef2, 0xf0513d5a2f99f },
            .Y = { 0x94dd3be9db46e9, 0x820409f00bec3c, 0x106aedcdd41553, 0x317ee63ea5232f, 0xdea20ed250f613, 0x4fdee7978550b3, 0x46245dda61fbd5, 0xfdbc211d79b20d },
        },
        {
            .X = { 0xcdfc798c2e0d9f, 0xc83c3b89a35e5a, 0xaec0e338fefcc4, 0x6c8601a8c38515, 0x35897b19765098, 0x6b3469ba4822f, 0x3e138a6eb8170e, 0x1fe432d5b37d46 },
            .Y = { 0x461e02cb5b82a2, 0xc9f2dbfc0aa83f, 0xe05689d9b1e4f1, 0xb6e7d8f25250bc, 0x3c50e127d1f9f7, 0x79e8d885ae0973, 0x4733a3dfa6369c, 0x75cae2264e1c75 },
        },
        {
            .X = { 0x632578ce6638a2, 0x3769e0de00201a, 0xf2164e517ae184, 0x1173bf21b449cc, 0x3100e27649ee93, 0xb00dd67dc588d2, 0x26cf171ee97616, 0x219e181504a2f9 },
            .Y = { 0x598c5fe02dbfcb, 0x797a5e49550409, 0xe63f4a0e934b7e, 0x2a7970f7908942, 0x71c55407ec4652, 0x1409ddf8301ce2, 0xf591e08bce978c, 0x3176f708e44f98 },
        },
    },
    {
        {
            .X = { 0xa199b8b3ece6a1, 0x63d8fbadef157d, 0x7160e10dff3c2d, 0x93b9b7b3dd15cc, 0xb96df9f3dee1a9, 0xfdbabbb51eaf4, 0xf69038fdc6b06f, 0x1d47eab2dcadbf },
            .Y = { 0x90c19459c37c89, 0xd8f33620dd9bf9, 0x8ab454ec0d347f, 0x99a31addcef778, 0x2b4b1c0cd40107, 0xc2f958c4d3dae9, 0xde90862539cc1f, 0x6db7904d8730b2 },
        },
        {
            .X = { 0x7a95c10184089f, 0xab2b551360f1f0, 0x4cd8fc9bfb93af, 0x47e9db360c6d5c, 0x75d2850cf39b02, 0xe1edfc9efd9974, 0x350ea5740cf572, 0xca581e4854cc4f },
            .Y = { 0x8571a46c449953, 0xc1c28c44a8f621, 0x4e8ffabaecc4bd, 0x3a5da784c19d2f, 0x312c13a819cc10, 0x9193bd3dbf50ed, 0x7f47c7071e0fa9, 0x8fbed2328c27cf },
        },
        {
            .X = { 0x7191b7b1efa703, 0x168270075b93cd, 0x44f94be4be37e8, 0x1e3cee76ad3537, 0xe94c78166f64a1, 0x699e3bb46b88ac, 0x46416329d8053f, 0xd21c8137246b3d },
            .Y = { 0xe5bf93a35e620d, 0xf51276279d66b6, 0xa6ac3fedcb044, 0x145dbfd89b5fbf, 0x427fab0a2f13cb, 0x665bbbe1f00646, 0x84a2f8a4fbe34f, 0xf997b5436d4d1d },
        },
        {
            .X = { 0x18ff4003a4a2b9, 0xdb53682a7240ab, 0x36abd85af521de, 0x644664c9470c4c, 0x5a66250c1e406f, 0x669e35985b0af4, 0xd8246fff9a13cb, 0x4673da4f0d956d },
            .Y = { 0xcd0f4683a61011, 0xaa9ce71a54310f, 0x1410959df0e2ec, 0xae39e3cff867bb, 0xe948df28e26879, 0x9f8416e1220fc, 0x453bae1b25fc5d, 0x39e905a1b3af54 },
        },
        {
            .X = { 0xe15507d1b98f5f, 0xe34200e477e74d, 0xe72e4d800524c3, 0x361c539ad8d047, 0xb99e63bae8fcde, 0x457b8837870b45, 0x97707a92d4a3ac, 0xc0fc6ccaf853ec },
            .Y = { 0xb43b343e61528d, 0xffa03cc344ad93, 0x283f302a02c777, 0x9ed2e0bbc4ffe0, 0x3f0aa9eb5447c0, 0x739490961f4740, 0xf05d6f25a97212, 0xdd48e8f74af168 },
        },
        {
            .X = { 0x2ec28109aab665, 0x6bf8db8b2943ef, 0xdab8b66bfdb4e6, 0xf483d928583dbe, 0xa04de22fb03852, 0x8e39df636b6bb, 0x6899b5e5f984d5, 0x1f5ac29a5c285e },
            .Y = { 0x91a8348d44d7eb, 0xa9753b8ec9a68b, 0x2738989ed190c1, 0x8fd35e41c66987, 0x971a87d3977634, 0xdb65ac311a3ae6, 0x1ef1d199bf0514, 0xa3d67ff30ea771 },
        },
        {
            .X = { 0xc895926577715d, 0xff25842cac582f, 0x34f716906072a5, 0xd77ffdf34db28e, 0x20cccdae7abb17, 0xdb7681d33b044f, 0x8e723d63ee4fc2, 0x19730107ae41d9 },
            .Y = { 0xfe975ee2c34b36, 0x87870373649fa5, 0x22ddc5232d724, 0xa7b2bdeb18f94, 0x8cd25a15c5c20e, 0x15d1fc6dd688e, 0x1d67aec9e73e07, 0xe04d839fa720f6 },
        },
        {
            .X = { 0x222f88afa1e4b, 0xe74e9fa5aa290c, 0x76f72568411f45, 0x313c60e534f90a, 0x7e6c1eca43ecfb, 0x3ff4bbaf8a1aac, 0x35cc5b29efeed6, 0x3e3043453ae9dd },
            .Y = { 0xc9f7a28f2a0d1f, 0x7661ff47f43245, 0xed997f674b2460, 0xa9d9fdb3c31057, 0x30bb638ff4b21, 0x25bbd7e74aa6cd, 0xf15e1aa00f4ff3, 0x6312708d2387d0 },
        },
    },
    {
        {
            .X = { 0x49c63c00a33b7c, 0xb42f6bb51af27c, 0x203c1d6a5a2e09, 0xe5077dddf31d11, 0x98bc740a7d07e2, 0xdd0cafa7eead9, 0x1ea1032db261ea, 0xa70967fd5bfb8 },
            .Y = { 0xb731bc68025ff9, 0x8835049687219d, 0x72c8fd4727f75b, 0x609c4af78dae16, 0x52949d17ae0da6, 0x3ea6b1fcd29a5d, 0xb30e8856202976, 0xe8cda67b05e601 },
        },
        {
            .X = { 0x73afd72068f840, 0x9e449713d61847, 0x1e2830936e521c, 0x7435bde8a352b1, 0x15914050ae3610, 0x170bc311e59d8c, 0xd9d0b23f5c2d5e, 0x8a36c484bb64b1 },
            .Y = { 0xe197387a04ade8, 0x8f9f968acc89a5, 0x58e945debe7642, 0x59cf28e67a4615, 0x6e3e9e4236c59c, 0xa93adce4cc9b48, 0x1112893dea9775, 0xde72a430fe29f7 },
        },
        {
            .X = { 0x69577effac8165, 0x47d4b1aaf206b0, 0xc58ae274e52f71, 0xc57575d7eefcb0, 0x52f0c8da836d84, 0x84342a28c2efd8, 0x3169346246c6bc, 0xf4852f81299ac8 },
            .Y = { 0xc62ee1e283728f, 0x7d96e08032a726, 0x4d1273f3232ec1, 0x370316c2fe5439, 0x16e3d21b949dca, 0xbea986cc0952dd, 0x4243941856211d, 0xb4d02532d4aab8 },
        },
        {
            .X = { 0x85707eafc657d5, 0x5c62eb6d8b289c, 0x252ca219c177ca, 0xb6ef86df3391f8, 0xb8ae0985feaa84, 0x35cdd244e521eb, 0x679d412bc11b84, 0x7a23a898fb1842 },
            .Y = { 0x84c41c17896a53, 0x4ee151adc7d6d0, 0x3f6fe1cc5d8080, 0x75e98431ae1d7e, 0xfb7dd63046b71f, 0xa2e72c1253ed39, 0x55fbd6a8fcefe0, 0xf7a18393a598f0 },
        },
        {
            .X = { 0xd4de2389294a9b, 0x2606651e4b4d4a, 0x2734ea99b88a8, 0x491e323ce077d4, 0xa62f120a749317, 0x421b8764d7718f, 0xb631a2531b232, 0xc910b2b557ea76 },
            .Y = { 0x2aa6d9f0f84678, 0xd65a2bc1213924, 0x21d8da1f0fcd73, 0x50b0f828433b76, 0xe3c938ddb82a6e, 0xfac63d029062c1, 0x2062947e4ea175, 0xf5f5e7f4cf0c5d },
        },
        {
            .X = { 0xe2a3ce588328a4, 0x6a960ab42abbc9, 0x89cebfe56af13e, 0xa8a248b7588da6, 0xe1a352469185e6, 0x48327a6bbf3027, 0x909bdf6fba3f6, 0xcce33b6b0e33ce },
            .Y = { 0x7ffc4d2728ec50, 0xe0f7782c64cc76, 0x9f69271b6d8ca, 0xffd4062c994e6d, 0x5e4e9c15ed5876, 0xdc4f2602158a7f, 0x5ea98eb2186e44, 0x196fe6b0d3cf5 },
        },
        {
            .X = { 0xd937777453355f, 0xaf15a5bb0f574c, 0x5d9a32ad5822d0, 0xf81ab86ff9f085, 0xe2b2c2c991da42, 0xf0a681c7382e8e, 0x65e4dfe3851c61, 0xe5afaff1fa467a },
            .Y = { 0x615705f0022d08, 0x80ed8cace9fa7a, 0xf36a1e89a532c0, 0xc6e31536e518f8, 0xbe0a21460e5a75, 0x8d2a90fc96c6e2, 0xea3744e13e3fe7, 0xfe3fcd6c275c2b },
        },
        {
            .X = { 0x2536f3e3364eee, 0xa8364d1866f5df, 0x808fb114bf750b, 0xde491f604bacb5, 0x84709184c2d536, 0xb09d73f743fe15, 0xa4c46360e996cd, 0x41637f359091bd },
            .Y = { 0x6bfda47b866d64, 0x249a2449da732b, 0x438f66b987339, 0x882a2da3154e5e, 0x26b42196fd2265, 0x2d3bf73b559eb9, 0x7d7e26d3bf55e8, 0x4b5224bd544792 },
        },
    },
    {
        {
            .X = { 0x77ec6bb1341589, 0x193229d6591057, 0x6b97dba69aede7, 0x9ba28da9cb2569, 0x3db94a9d85b96e, 0x8d77ca4bf9002c, 0xb5ef5b2fa4462d, 0x3c13cbdf044e9c },
            .Y = { 0xbabb0f6ff5af6b, 0xbde2d0f8590a77, 0x827e5ad95cbcb5, 0x1c1152f485f935, 0xed0d4203fa6a3f, 0x22afb9582f5ed7, 0x1e12f816627429, 0x964a09b4d0e313 },
        },
        {
            .X = { 0xb439c2fa6c6905, 0x187f594f5d2058, 0xbbfefa312653ab, 0xf52a366982a2f8, 0xa59304c58a66ea, 0x6db77946c4bafb, 0xcf56590ceb33ec, 0xa29102f1538158 },
            .Y = { 0x6ccb3a97a6f0e2, 0xfb0e5267cf0126, 0x19b247dbc73aa1, 0x2cedec1e74394f, 0xbb7ed9ec6c6809, 0x7b53f92e062667, 0xed43f801a5cbaf, 0x98fe98ef2cb30a },
        },
        {
            .X = { 0x584497394da9b6, 0x8112b714219e37, 0xb928383b6a0d63, 0x78d205f5699ec9, 0x7c4d535dd5027, 0xafb6fbdc0258be, 0x99192df3448dcd, 0x5aae60914b7034 },
            .Y = { 0xc52faf3545297d, 0xaf0c1348fd5f4d, 0xcf1fa9d4d2163a, 0xccd2e542df114d, 0x2fed9090296aac, 0x70dbf1d609c1b1, 0x2916fc3f36fb99, 0x1db112c7369ba0 },
        },
        {
            .X = { 0x1e2e45cdddc4d9, 0xcc0ed9ffe055fd, 0x9e1911aa182d4d, 0xa173cc04c34365, 0xf2a9a452d80f4, 0x69a27f3e7ea604, 0xcc5bdd7d9d6154, 0x6fe0ad132a97b2 },
            .Y = { 0xb70221959574fc, 0xdccbcbc1c56d46, 0x29689d15858ec0, 0x178e2cd111568e, 0x5ff43f8ad09e06, 0x575efae24056e, 0x5d9a770ba9059b, 0x6e99977d98caca },
        },
        {
            .X = { 0x997200341145bb, 0x19ce18ed0544b7, 0xf5f0f9e3dc19ce, 0xe1ba1c009a1a1, 0xa1e947375340be, 0xc4e58f80449969, 0x6de9031b16f290, 0x72f1ad42de1a19 },
            .Y = { 0x2c369c7025439f, 0x94df0d865872b0, 0xde53ab3569d247, 0x2adb855fe75ee2, 0x6afcabcd93fd57, 0xefd918ea6fcf0a, 0x8ac06e2b504fc6, 0x68939949ae9f47 },
        },
        {
            .X = { 0x3e356e21ca4ddf, 0xbff035b3bcd853, 0x1eaef0627eb24d, 0x9b3ba14612598f, 0x3e61cf9deb5f13, 0x20e286a8f13670, 0xf39e451cbae8b6, 0xdf4644f54062d5 },
            .Y = { 0x700a8d9e312105, 0xa037afa3fd6327, 0xdf881eb2090f05, 0x1d543ed8503d12, 0x89228511d80027, 0x3f66ef5e1a7ef2, 0x1685806d21babe, 0x991453a8bf5701 },
        },
        {
            .X = { 0xe560f630c65070, 0xae78e8d9439a02, 0xd36dbb349d9165, 0x861ce12f705f95, 0xf3f8619c913dd0, 0x761362c58302b, 0xb9b5c7114e84b7, 0x6443b4eb595c33 },
            .Y = { 0xcef3c9d06929f3, 0xa70d93ce896024, 0xa5b25cbbac8708, 0x97555e95480c9a, 0xf29155a3368e17, 0xb579285ca4ebd1, 0x81dae3df611cf4, 0x133c7c1512cb37 },
        },
        {
            .X = { 0x3a32e13e056ce6, 0x28929f83985892, 0x3dca561888d7a9, 0xb2c05ad05c871f, 0x5fca7a34333060, 0xc64c1a411e6b27, 0xc32c035d17a501, 0x937ed14f513528 },
            .Y = { 0xccee8a2fb3c019, 0xc0d85394d221d5, 0xc32b0be99e2317, 0xf3e9f1d28f5c19, 0x85102f4e0bd208, 0x79333070f44117, 0xe2e91a2534acc7, 0x8081caa65aa478 },
        },
    },
    {
        {
            .X = { 0xcde508c16d1ab3, 0x4f2a8f2c6e3f8b, 0x83296cf3f0c7a, 0xba2c39c092ef4, 0xa6c31c4b8d7861, 0x6228a1a857d147, 0x9afe0c28d85391, 0xd5359d9cf704c2 },
            .Y = { 0x9cb291e843b36b, 0xb69b8a3baf6231, 0xe395a2dbe69114, 0x81dda456e66416, 0x612792a82ad88d, 0xfd87ee01250fd0, 0x1026887db00ee9, 0xf318874b6ff24a },
        },
        {
            .X = { 0xff77119118fd09, 0x112577826dbe4b, 0x25a822609c219b, 0x4fcd13c971db1e, 0x9fd8ec0009748, 0x7dca1a19f0c9f3, 0x45cd7812d19868, 0x3c6d2402613fc2 },
            .Y = { 0x39fd300effd6b7, 0x31546fd34bccf3, 0x26d8699173a058, 0x4a2ccbe8d0cd0c, 0x2b740bb9523c06, 0xbd2013d9fbdcce, 0x1d0a83aa836b94, 0x197bd969bf299c },
        },
        {
            .X = { 0x4bfd555263b3d1, 0x242f0b7e577485, 0x2cc55c56ce209a, 0x40bc95551a3b8c, 0x3def49c57859a7, 0x97d99036d532a2, 0xb3b77b576b08bb, 0x7ef2228e4ed494 },
            .Y = { 0x139f8afbf8fba6, 0xaa89d09c47d63d, 0xa632e4f9abb23d, 0x4855c3a4c55fb3, 0xe9f90292427d16, 0x1055bc20563e9, 0xc0752f9d2a7c14, 0xdd4fe309d16b35 },
        },
        {
            .X = { 0xf1488ea1868934, 0x436eeb9046a10f, 0x962bac844a9154, 0xc23e47890ce650, 0x86b6f382f66e3, 0x2004ce87ddd8a1, 0xa970df5ad1e67d, 0x74a50a9c2a2924 },
            .Y = { 0x5193b35816ba0b, 0x9bf72eabe5dd10, 0xf6f204cdf35209, 0xe6d2328f131318, 0x5ad2d352bfecd0, 0x7251b0b713483c, 0xe28a1ae28f8de7, 0x1849866f2fba9e },
        },
        {
            .X = { 0x6828803c41d73e, 0x5f52bb4d04301b, 0xd47d3256c292ae, 0x1a5bd6048e60f8, 0xb15180968b6a59, 0xb0a108a5468e1b, 0xb0941be04c3aa4, 0x472dd59b80de1d },
            .Y = { 0xe3d4bfc2859cdb, 0xc5f3acc8279d76, 0xb21433e23eada3, 0x1b620c095423fe, 0x7d4c6154cb2324, 0x929aa02f7c0374, 0x8448f1b5ef35d5, 0x7d1137a76ad5fa },
        },
        {
            .X = { 0xd4d18dbd7e9505, 0xe179fdc5a76472, 0x40fc8bc5aef076, 0x3bc37df9866fe8, 0x1cec0d7831adbf, 0xec58838ed6cda8, 0xbd90dbab8dfb1f, 0xaf0198d6c5e391 },
            .Y = { 0xede81f474ad0d2, 0x3f0a7e2ecc5524, 0xbef0ab4406d0b7, 0xd88132e36ea6ce, 0x86abea4e64f4f4, 0x6fdb9cffe89687, 0x53e6cfc3a3feac, 0x6ce96917503b7 },
        },
        {
            .X = { 0xed7d0d9e08d2da, 0x164e311541d8b0, 0xf8ce102c2371ba, 0xf8a0d2038b8f2e, 0x8be2e5f4184ef6, 0x6536a227aa1d84, 0xde2b9915abf836, 0x4adcd937c25be4 },
            .Y = { 0xf548fb5bcb9652, 0x734a3e540b18cd, 0x27f0599535873d, 0xd7475a274be0b5, 0x636bb7f80e035a, 0x183108628f6181, 0xe2663eeebee421, 0x1636602923af2f },
        },
        {
            .X = { 0x9e4f96da35057b, 0x262a02559c455, 0xc6c9a3af690767, 0x799eac40b7e67e, 0xa75ac44d1c057a, 0x7aedab28460099, 0x7ddfeac1492042, 0xd6f0c16e3cfb86 },
            .Y = { 0x56f328895571d2, 0x79f34e01eb5683, 0xd2958afe2e61a6, 0x3022cafdb1cc33, 0xcadb6cf9eed482, 0x3034caef19f438, 0xac2ec363d88b5, 0xbc822c74c95f16 },
        },
    },
    {
        {
            .X = { 0xdc9a373068ab85, 0x85c76c278946c4, 0x480c9e8ec59d0e, 0xc03527d00292d1, 0xdf72655d2662eb, 0x3e1e1924efb2ea, 0x829d1ffa461ecc, 0x1fe505a99a3a9e },
            .Y = { 0x115de09e387bae, 0x4c8ea64a32b5dd, 0xdfd594f83bdbe6, 0x826e2c449f2043, 0x5f3c8eff616bb8, 0xf6829ac99b6220, 0xe3cb14d580f8c, 0xcae675403c8b92 },
        },
        {
            .X = { 0xbbab14991ec863, 0x6ad32e8f177b9e, 0xc8fbc55acaabe, 0xf4475e23be96d, 0x110bf690954a2b, 0x5816c4596e1fb4, 0xb59bc886906d98, 0x1c86b9ec355008 },
            .Y = { 0x72424386f5bff9, 0xaf4093646944a0, 0xf5dad2cd5570ff, 0xd181bdb47c793, 0xcb16b9ab1c39bb, 0xa5bdc03a2cc9eb, 0xc4bbdfb9dee078, 0x3dfebd5c802b34 },
        },
        {
            .X = { 0x35fa3bc66bf4e6, 0x684229b48a0d7, 0xb67f9d7c6f6d3a, 0xbd9fc7580695c9, 0x2154af4189d319, 0xad52c803f492f6, 0x77595c98512bd8, 0x4cfadba1ecc471 },
            .Y = { 0x3282eb4caf6816, 0x94bbce1125a1f2, 0xf4c1f468cc1ac, 0x79c66b100e4f64, 0xfb170b51090338, 0x6cc465591eb8da, 0xbe744eec707a11, 0x427f723c618ef5 },
        },
        {
            .X = { 0xebe50893e1ab31, 0x700f85862085e, 0xf5786a8cd2f46f, 0xb363ce97922b1, 0x7686069a13209f, 0xed27def28b71f3, 0xb42a4fd70c8317, 0x3e22c3e02d404c },
            .Y = { 0xd513862b426f51, 0x685a962ed6df8a, 0x490688c2e6b011, 0x698b8ab1377a02, 0x851322719714e8, 0xa5b3f0b98c1664, 0x9ea537f5a1ec2f, 0xa022d85d25caa3 },
        },
        {
            .X = { 0xde7aad41271bec, 0x397b0660f2aba9, 0xf2b57924d387c7, 0xe89d76cd9a26b3, 0x4a281d131ea8f5, 0x6dfd65a1ff7d73, 0x4942310a51a307, 0x6ebbb4d78be9e4 },
            .Y = { 0xc1a70073ad91fe, 0x98a33b7d188da5, 0x44175f1b5de62a, 0x77b138cb63d26, 0x2c8e3de3925c31, 0x21ad2563bff9f2, 0x836d29f741c7d5, 0xfc239323923eb8 },
        },
        {
            .X = { 0xff0d4f54b8cdcf, 0x99c9f4694c8946, 0xcb201e369918dd, 0x2666aff6a0dec3, 0xec12c1b5bdd5a7, 0xa5a111978b7f11, 0x1185aeba59c231, 0x74a3438d5f2abf },
            .Y = { 0x2ff34ed0b8afcc, 0x92929d95f4937a, 0x2502d281faa2f, 0x52a318478e27c2, 0xb8d194396625bf, 0xe6e92c21943ae9, 0x9f4fe31f644209, 0xba7c8de6d2fdf1 },
        },
        {
            .X = { 0x96c752e050e317, 0xfaad7bb11b1f49, 0xe4600a0e5b3925, 0xa00e1177553fd6, 0x5cf47728f1995d, 0xa49253f44882a1, 0x98feef71a6516b, 0xbf6b425dbbd8ff },
            .Y = { 0x59a5981962a024, 0xea199033e548e6, 0x622632cdeceb44, 0x47a0a994705907, 0x1074f8b579b168, 0x7c6708e92b7657, 0x600c57b4b14b7, 0x806d6523226547 },
        },
        {
            .X = { 0xca2e414491db52, 0x214b07b8edfb1e, 0x843da2308a97f0, 0xdce9bee1cf8e5d, 0xd464fbbf2e3988, 0x47f78074aa0dc4, 0x625365392afa18, 0x7828ab61388c91 },
            .Y = { 0xb4b99ec363cd4e, 0x4cf08ec5f8a13d, 0xd50b05fe6dfbc9, 0x8dee6b3066c319, 0xa36c2789f530cb, 0xfc2b09fd9de80d, 0x5cb94215e7a661, 0x7018b98621c6ab },
        },
    },
    {
        {
            .X = { 0xb00c86dbd88219, 0xe24d6a36897ba2, 0x62471ac80ea717, 0x63317a1ff1cffd, 0x61798b7c315682, 0xd997699304a1a2, 0x5d1a4f35d800d3, 0x7f086f59ac55af },
            .Y = { 0x78e9be15d321af, 0x8874fcbf186426, 0x3190280e5003a6, 0xe8025b6f5c6eba, 0x87066b8caea7e2, 0x939f0f5893ff61, 0xe806645eea5931, 0x6bf7e58d2cbcd0 },
        },
        {
            .X = { 0xec23d28c58a4f3, 0x72d021b4ac5f65, 0x7c110577c90589, 0xacfc9d407f8f7a, 0x3e3f416eed043a, 0xf212c8224834aa, 0x3a36b592a2dccc, 0x312ce76b84988f },
            .Y = { 0x6b5ba73796db3e, 0x3302ff18f2fe40, 0xff79942567095f, 0x73d1714f70d853, 0x5626ab5e6abfe4, 0x2b7ba23f156623, 0xa3d6c9db1de51d, 0x84051155429dfb },
        },
        {
            .X = { 0xbe72d0f1eb5155, 0x2a132f84eeed0a, 0xc1ed7bc8bc1279, 0x948da8bca2b1ae, 0x5fcd9c022eee72, 0xea068181a11ab1, 0xbbe343675518aa, 0x19add933bab529 },
            .Y = { 0xf64d706b62bf61, 0xb94cb668d733f2, 0x16d65c355a7fd1, 0x59ea37529b0d74, 0x343c78a921c004, 0xcd761a56d05b3f, 0xde6112b1e21de6, 0x524738974c66e3 },
        },
        {
            .X = { 0x12f2ce67e97b2, 0x4a849c746d2a1c, 0x2ef0baa684fff6, 0x9a6e51fb4d1333, 0xc98dabc863fe7b, 0xb58019a7e31d7f, 0x80622605a2eda3, 0x6e7455cb7bca73 },
            .Y = { 0x12b47d60d34589, 0x2ca57e9d6fce37, 0x4d1c1865622b44, 0x215eb01014605b, 0xc7ed2467a1c0f0, 0x37c6ba2cc9faaa, 0xeecab111c7db55, 0xf5d748711c4ba8 },
        },
        {
            .X = { 0x9940869c0a748b, 0xcee462eb5381f3, 0xe2d62ca3a28f26, 0xe7fdb03203e055, 0x7ba80a735b2bc1, 0xf0f2d9dc2ba96e, 0xe1135fb41e8863, 0xd7106a82ba89cb },
            .Y = { 0x916930f75521e8, 0x4783370ae36fdd, 0x223132ffa1e7b8, 0x474a0c472806ec, 0xc0a201d71c8ef5, 0xb6845d6ac742ca, 0xcefb9f0e97aa91, 0xb488e41c001055 },
        },
        {
            .X = { 0xe51cda693676e8, 0xb0220bf31b59cb, 0x1bb118b4b18e07, 0xbbf0598453df24, 0xa71ddd8a41619f, 0xbe21a3d6c3c031, 0xa92c11438a3893, 0x8536a4815a2825 },
            .Y = { 0xfb462122c1698c, 0x9f0f22ce52f799, 0xabbc62311521b9, 0x69cffea9605f22, 0x213f49bba0a291, 0xfc330e3e152639, 0xa937ff8b5e8852, 0x654b27999945fc },
        },
        {
            .X = { 0x6a9e38848d29e4, 0xa7c01a8fa361a4, 0x812e4a4c2ac5f7, 0x1c0f76d39474af, 0xb9ec1c80ad8de9, 0xd26a2f3901d48a, 0x728da0d4295714, 0xb4fc20d16e3403 },
            .Y = { 0xff79581f14767c, 0xbc6fb7bbf243da, 0xf875eac89cee8b, 0xcb0ce617068cb4, 0x1184e7439521ac, 0xa8308be50bf0ad, 0x605648e8b9595d, 0xf54d12210d89b2 },
        },
        {
            .X = { 0xcbb70377202c1a, 0xa54f340433a205, 0x38dc84faab9df0, 0x5c9dde204498be, 0x11a9695b622816, 0xde102fe999f820, 0xc4b286e429fcd5, 0x470738dea13a4c },
            .Y = { 0xc0492cb150e9de, 0x8283489d2cac66, 0x71ec53e8c8604, 0xfed435181b096b, 0x950f82d0661de2, 0xdf5c9e9f4abdbd, 0xf4973241dced45, 0x45a7b3a4973db7 },
        },
    },
    {
        {
            .X = { 0xb92e5ef7765db2, 0x37e88fa78e14c, 0xb45a3c7d983c2e, 0x6518ebf6f7d77b, 0xa53a7bdefc8ce6, 0x271261a6e0184, 0x6eabf5180012c4, 0xba568ef72d32 },
            .Y = { 0x36bcdad87026de, 0x7f079ad3cdf070, 0x6d95abe3d3f6fd, 0x5fdf276ab5bbb5, 0xedcc23e0b0da3f, 0xff455b5f7719e7, 0xae2c90ddd7fcc3, 0xcfe050dd4fa8c7 },
        },
        {
            .X = { 0xcc6aa53e724653, 0xaef0ff44a5727f, 0xcfc1b26c2b9e69, 0xb0d15d8fb73937, 0xe7c343004b6174, 0x49aff809d6ac73, 0xadbfcf5b8bf512, 0x13e3d29fd1877b },
            .Y = { 0x280ba7f96c19dc, 0x94e55abc3a0bc0, 0xf2206618aa02ee, 0x274eaabcae8e7b, 0x339482b9150b3a, 0xa37c18e9b947d9, 0x84e777ebe2e373, 0xffe489c4e0188f },
        },
        {
            .X = { 0xcf204ade5494b0, 0x5ec366518f8e46, 0xbbbcf55c968498, 0x8e940a8ba0bc58, 0xdb3ccf98831a65, 0xac6a9a2d17dfa0, 0x573a9780c26fa2, 0xb5ee5c564aad31 },
            .Y = { 0x40cf6c37c5102b, 0x2d0f863275c114, 0x93e5e33c22e26b, 0x5420bfe3aaf0be, 0x5bf4216d2c941a, 0x1000a7bda0aca, 0x3e14853d490294, 0xd8de35e029e3c4 },
        },
        {
            .X = { 0xadb79a92b76d91, 0x77982e11b350aa, 0x17714b3cf035f3, 0x91ef462bc0f994, 0x6c1f925416d11, 0x16810846dfd10f, 0x995145e6013042, 0xae9ddc921ab2ea },
            .Y = { 0xc185eea9bbb893, 0x786a57f43f56e0, 0xc9d68c04043a88, 0x846fd8230bcda5, 0x9736a998e91b6d, 0x99573070023aaa, 0xd501bdd42303b4, 0xb2de7066034b8d },
        },
        {
            .X = { 0x99cd0ef2e06d86, 0x4053619c827a46, 0x7284b4da105c34, 0xfe0fb58ae3fda0, 0x8f4ee29376143c, 0x961d971e662e64, 0x1cb5993d93c96c, 0x103ae59645ad50 },
            .Y = { 0xf75042e1f5babc, 0xf298e08a9b468d, 0x3d2ff413ac5f65, 0x452a8d5d799dfc, 0x5f3943cf6a9a6a, 0xfc90ce771cfa62, 0xd65d2e5dab0ebe, 0x894281943c1ad4 },
        },
        {
            .X = { 0x20e3965c3104e8, 0xb7c93f898416c7, 0xd8e77ecd599eb2, 0x6ed2328bdb009, 0x7ea8639dbf04ea, 0x1278e3904570f9, 0xb2eafb7c0947de, 0x144c717592a694 },
            .Y = { 0x993bd7d7daf382, 0x6755a4a82aeb37, 0xd513f27ad11ed1, 0x6b514399aba25c, 0x1c8d5277c42a72, 0xc004b0ae3d4c47, 0x69c3488ea04391, 0xcad9d8f88ab52e },
        },
        {
            .X = { 0x8153f13c711ee1, 0x896ca84502ee79, 0x803cad7616b68e, 0xa2b9b9824f6dbe, 0xcce207e3bc4ec9, 0x948ad34f8a9679, 0x5dd713eae907ab, 0xdc88a39896d4fa },
            .Y = { 0xe84175d0bfba0d, 0xeabc17c8d301e2, 0x1601b6b72b3bb8, 0x14f4f4e397d2ad, 0x2facefc9ad2d46, 0x8773fdfda1f45a, 0xb7659c3023caa, 0xa6d75983b1af93 },
        },
        {
            .X = { 0x234f0b02bab274, 0xc9e58f3311fedf, 0x9c4997c12ac25e, 0xfbe3e3988b5d97, 0x943a5bbcbaa35, 0xf36853a9ed8aab, 0x800c532454ba1f, 0xacb9acfdef176a },
            .Y = { 0x937c291b855ca9, 0x340748857b3905, 0xaed2edfa18ac8e, 0x8c17d86f30d8c4, 0x11fec9c439a2c0, 0x68f5f486cd17d5, 0x5a8557bd3b9ad7, 0x24d1a321251858 },
        },
    },
    {
        {
            .X = { 0x2c9532fc4d0c94, 0x25c9b41d9d5d0b, 0x12678e093e9be, 0xfba8084e1ae73d, 0xaeae8837916f1a, 0xed39f12a9490d6, 0xd9f712ba566ec3, 0xb643f45c7da4d1 },
            .Y = { 0xd4d2b46f07deb2, 0xf756519b42c2d4, 0x871cef91903427, 0xafbe3f99b361f4, 0xbdde5250664076, 0xaf41294dbb0314, 0xd89a58a971d088, 0x7fcabe31867a62 },
        },
        {
            .X = { 0xbd8755a51251cc, 0x5c9c00ce4ce98a, 0x96215e6a2ff7b8, 0x8f76df9634b8ff, 0x3497206529089, 0x3773ae3ae39c55, 0xbb2c0f42f0864f, 0xe207306a50cdcf },
            .Y = { 0xa2d83555c69168, 0x73d2f51bcfc991, 0x52d67e90747619, 0x827f65d22bac1f, 0xfade5937e4663c, 0x6323267474cd12, 0x61930394374775, 0x16fd8bb482960a },
        },
        {
            .X = { 0xe8282bf4d89da4, 0x3536435e50ce6d, 0xf4e63c0869b15a, 0x81f01264e1293b, 0x7c54ccc4fc1cb1, 0xc8cc98bf4fba9c, 0x5cadb4dc02fe0a, 0xdb46926732578 },
            .Y = { 0xa99c1de9fdde1, 0x76e9fa906c5891, 0x8e5d7f6e51e729, 0x4376dcb15f52f7, 0x83ab2e2202a522, 0xac700103ed0672, 0x374c6173264e9c, 0x241cd417baa09d },
        },
        {
            .X = { 0xf09bdfa0ec9d1a, 0x8584f7f485cd1a, 0xbe2a1970c1e9e5, 0xd87cd187e1da88, 0xef296ff0f55278, 0xc95391410477ea, 0x2ea6d64440bb1f, 0x9f85fbf721f37d },
            .Y = { 0xddb06564a7c9ed, 0xe0f6ba75df560e, 0x6101cb63709d33, 0x7494a3428fcb78, 0x57ac2dc88fd13c, 0x79de7be4504f03, 0x6912819d519d16, 0xb893a5e59bdd8 },
        },
        {
            .X = { 0x30eeede981901, 0x598cbeee10e08b, 0x69e10be6b1b5ca, 0x2ecd4d388a8a19, 0x30b32da71be42f, 0x9a7a12877714dd, 0x429281ca094369, 0xb23f67c0ce9ff9 },
            .Y = { 0x37b6fc6255b50a, 0xbd954fb8b15c04, 0x6ef66ade25d729, 0x3c2900f6e6a121, 0xa288a113407d01, 0x82779c44b50243, 0xe62ac33ea3d162, 0x625b0bdae6a4db },
        },
        {
            .X = { 0x2aa8df52c7d68d, 0x81712ceda9905, 0x6283fef257cafc, 0xe46a27e9ab6d7b, 0x679c17fd239254, 0x98d234fcd44db6, 0x5c77c15462e130, 0x977f977f766342 },
            .Y = { 0x363e081185e967, 0x50bf845c6b523, 0x91be36fac2db28, 0xb994be1a92f500, 0xcbb4641f6d5b91, 0x1e04ebf2ac2bc7, 0x942c3ddebb3690, 0x360da8e8e9e95a },
        },
        {
            .X = { 0x67de56fde0cffa, 0xd26ca0530e4967, 0x492c6001daf922, 0xfa2922e5702997, 0x9c45bc5f0d4ed9, 0xc41643c12f5979, 0x1280426bf99bcf, 0x45d0d91ceaacb8 },
            .Y = { 0xcc13865a645204, 0xaa3ff77d33c389, 0x38e47fecc7ebfb, 0xd61fa1b81e7401, 0x6185f1babd7a2a, 0xc4305c4fcdb4ed, 0x40223246053ddd, 0x665627e3c7767b },
        },
        {
            .X = { 0xbb11f95b60e0ea, 0x3ac1e212e9a670, 0xcaa3b2f8607c90, 0x4397904fe4d694, 0xb19123162d37a8, 0xec6700db16a036, 0x561b298f01bd62, 0xef03d8c1c12749 },
            .Y = { 0x4c797f0813ad47, 0x930ec39ff790b9, 0x4f63a29d1d18f3, 0x317a97e0c418d6, 0xb728bc9dee6db8, 0x353d03a2a127f5, 0x3a24868d1c67ae, 0x69e8a9a63860f9 },
        },
    }
};
