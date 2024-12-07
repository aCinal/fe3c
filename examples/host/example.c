#include <fe3c/eddsa.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>
#include <assert.h>
#include <stdlib.h>

#define FAIL_IF(cond, format, ...)  if (cond) { fprintf(stderr, format "\n", ##__VA_ARGS__); exit(EXIT_FAILURE); }

typedef struct fe3c_params {
    enum { UNKNOWN, SIGN, VERIFY, KEYGEN } action;
    eddsa_curve curve_id;
    const char *message_path;
    const char *signature_path;
    const char *context_path;
    const char *secret_key_path;
    const char *public_key_path;
    int prehash;
} fe3c_params;

static void print_usage_and_exit(const char *exename);
static void parse_arguments(int argc, char *argv[], fe3c_params *out);
static void validate_arguments(const fe3c_params *args);
static void sign(const fe3c_params *args);
static void verify(const fe3c_params *args);
static void keygen(const fe3c_params *args);
static void *reliably_map_file(const char *path, size_t size);
static size_t get_filesize(const char *path);

int main (int argc, char *argv[])
{
    if (argc == 1)
        print_usage_and_exit(argv[0]);

    fe3c_params params;
    parse_arguments(argc, argv, &params);
    validate_arguments(&params);

    switch (params.action) {
    case SIGN:
        sign(&params);
        break;

    case VERIFY:
        verify(&params);
        break;

    case KEYGEN:
        keygen(&params);
        break;

    default:
        /* Should never get here */
        assert(0);
        break;
    }

    return 0;
}

static void print_usage_and_exit(const char *exename)
{
    printf("\n");
    printf("------------ Example application of the Fe3C crypto library ------------\n");
    printf("\n");
    printf("    usage: %s --sign|--verify|--keygen\n", exename);
    printf("\n");
    printf("    common options:\n");
    printf("            --ed25519|--ed448    Edwards curve to be used for the signing, verification or public key derivation.\n");
    printf("    public key derivation options:\n");
    printf("            --secret_key         Path to the secret key.\n");
    printf("    signing options:\n");
    printf("            --secret_key         Path to the secret key.\n");
    printf("            --message            Path to the file which is to be signed.\n");
    printf("            [--prehash]          Flag denoting the usage of the PH variant of EdDSA.\n");
    printf("            [--context]          Path to the optional context data.\n");
    printf("    verification options:\n");
    printf("            --public_key         Path to the public key.\n");
    printf("            --message            Path to the file signature over which is to be verified.\n");
    printf("            [--prehash]          Flag denoting the usage of the PH variant of EdDSA.\n");
    printf("            [--context]          Path to the optional context data.\n");
    printf("\n");

    exit(EXIT_SUCCESS);
}

static void parse_arguments(int argc, char *argv[], fe3c_params *out)
{
    /* Set the defaults first */
    out->curve_id = EDDSA_NUMBER_OF_SUPPORTED_CURVES;
    out->action = UNKNOWN;
    out->prehash = 0;
    out->message_path = NULL;
    out->signature_path = NULL;
    out->context_path = NULL;
    out->secret_key_path = NULL;
    out->public_key_path = NULL;

    const struct option long_options[] = {
        /* Basic action */
        { "sign", no_argument, NULL, 0 },
        { "verify", no_argument, NULL, 0 },
        { "keygen", no_argument, NULL, 0 },

        /* Options and parameters */
        { "prehash", no_argument, NULL, 0 },
        { "context", required_argument, NULL, 0 },
        { "message", required_argument, NULL, 0 },
        { "secret_key", required_argument, NULL, 0 },
        { "public_key", required_argument, NULL, 0 },
        { "signature", required_argument, NULL, 0 },

        /* Curves available */
        { "ed25519", no_argument, NULL, 0 },
        { "ed448", no_argument, NULL, 0 },

        { "help", no_argument, NULL, 0 },

        { 0, 0, 0, 0 }
    };

    int option_index;
    int ret;
    while ((ret = getopt_long(argc, argv, "", long_options, &option_index)) != -1) {

        FAIL_IF(ret == '?', "getopt_long() failed to handle argument: %s", argv[optind - 1]);

        switch (option_index) {
        case 0:
            assert(0 == strcmp("sign", long_options[option_index].name));
            FAIL_IF(out->action != UNKNOWN, "Specify only one of --sign, --verify and --keygen");
            out->action = SIGN;
            break;

        case 1:
            assert(0 == strcmp("verify", long_options[option_index].name));
            FAIL_IF(out->action != UNKNOWN, "Specify only one of --sign, --verify and --keygen");
            out->action = VERIFY;
            break;

        case 2:
            assert(0 == strcmp("keygen", long_options[option_index].name));
            FAIL_IF(out->action != UNKNOWN, "Specify only one of --sign, --verify and --keygen");
            out->action = KEYGEN;
            break;

        case 3:
            assert(0 == strcmp("prehash", long_options[option_index].name));
            out->prehash = 1;
            break;

        case 4:
            assert(0 == strcmp("context", long_options[option_index].name));
            out->context_path = optarg;
            break;

        case 5:
            assert(0 == strcmp("message", long_options[option_index].name));
            out->message_path = optarg;
            break;

        case 6:
            assert(0 == strcmp("secret_key", long_options[option_index].name));
            out->secret_key_path = optarg;
            break;

        case 7:
            assert(0 == strcmp("public_key", long_options[option_index].name));
            out->public_key_path = optarg;
            break;

        case 8:
            assert(0 == strcmp("signature", long_options[option_index].name));
            out->signature_path = optarg;
            break;

        case 9:
            assert(0 == strcmp("ed25519", long_options[option_index].name));
            FAIL_IF(out->curve_id != EDDSA_NUMBER_OF_SUPPORTED_CURVES, "Cannot specify multiple curves (e.g. --ed448 and --ed25519)");
            out->curve_id = EDDSA_CURVE_ED25519;
            break;

        case 10:
            assert(0 == strcmp("ed448", long_options[option_index].name));
            FAIL_IF(out->curve_id != EDDSA_NUMBER_OF_SUPPORTED_CURVES, "Cannot specify multiple curves (e.g. --ed448 and --ed25519)");
            out->curve_id = EDDSA_CURVE_ED448;
            break;

        case 11:
            assert(0 == strcmp("help", long_options[option_index].name));
            print_usage_and_exit(argv[0]);
            break;

        default:
            /* Should never get here */
            assert(0);
            break;
        }
    }
}

static void validate_arguments(const fe3c_params *args)
{
    FAIL_IF(args->action == UNKNOWN,                                 "Unknown action, specify either --sign or --verify");
    FAIL_IF(args->curve_id == EDDSA_NUMBER_OF_SUPPORTED_CURVES,      "Unknown Edwards curve, specify either --ed448 or --ed25519");
    FAIL_IF(!eddsa_is_curve_supported(args->curve_id),               "Unsupported curve with ID: %d. Recompile libcementite with support enabled", args->curve_id);

    switch (args->action) {

    case SIGN:
        FAIL_IF(args->message_path == NULL,                          "Message to be signed is required, specify --message=<path>");
        FAIL_IF(args->secret_key_path == NULL,                       "Secret key is required for the signing procedure, specify --secret_key=<path>");
        break;

    case VERIFY:
        FAIL_IF(args->signature_path == NULL,                        "Signature to be verified is required, specify --signature=<path>");
        FAIL_IF(args->message_path == NULL,                          "Message whose signature is to be verified is required, specify --message=<path>");
        FAIL_IF(args->public_key_path == NULL,                       "Public key is required for the verification procedure, specify --public_key=<path>");
        break;

    case KEYGEN:
        FAIL_IF(args->secret_key_path == NULL,                       "Secret key is required to derive the public key, specify --secret_key=<path>");
        break;

    default:
        /* Should never get here */
        assert(0);
        break;
    }
}

static void sign(const fe3c_params *args)
{
    void *secret_key = reliably_map_file(args->secret_key_path, eddsa_get_secret_key_length(args->curve_id));
    void *message = reliably_map_file(args->message_path, 0);
    void *context = args->context_path ? reliably_map_file(args->context_path, 0) : NULL;

    size_t message_length = get_filesize(args->message_path);
    size_t context_length = args->context_path ? get_filesize(args->context_path) : 0;
    if (context_length > 255)
        context_length = 255;

    u8 signature[eddsa_get_signature_length(args->curve_id)];
    eddsa_sign_request req = {
        .signature = signature,
        .message = message,
        .secret_key = secret_key,
        .message_length = message_length,
        .context = context,
        .context_length = context_length,
        .curve_id = args->curve_id,
        .phflag = args->prehash
    };
    eddsa_sign(&req);
    /* Write the signature to the standard output */
    (void) write(STDOUT_FILENO, signature, sizeof(signature));
}

static void verify(const fe3c_params *args)
{
    void *public_key = reliably_map_file(args->public_key_path, eddsa_get_public_key_length(args->curve_id));
    void *message = reliably_map_file(args->message_path, 0);
    void *signature = reliably_map_file(args->signature_path, eddsa_get_signature_length(args->curve_id));
    void *context = args->context_path ? reliably_map_file(args->context_path, 0) : NULL;

    size_t message_length = get_filesize(args->message_path);
    size_t context_length = args->context_path ? get_filesize(args->context_path) : 0;
    if (context_length > 255)
        context_length = 255;

    eddsa_verify_request req = {
        .signature = signature,
        .message = message,
        .public_key = public_key,
        .context = context,
        .message_length = message_length,
        .context_length = context_length,
        .curve_id = args->curve_id,
        .phflag = args->prehash
    };
    int verified = eddsa_verify(&req);
    FAIL_IF(!verified, "Verification failed!");
}

static void keygen(const fe3c_params *args)
{
    u8 *secret_key = reliably_map_file(args->secret_key_path, eddsa_get_secret_key_length(args->curve_id));
    u8 public_key[eddsa_get_public_key_length(args->curve_id)];

    eddsa_derive_public_key(public_key, secret_key, args->curve_id);

    /* Write the public key to the standard output */
    (void) write(STDOUT_FILENO, public_key, sizeof(public_key));
}

static void *reliably_map_file(const char *path, size_t size)
{
    /* Open the file for reading */
    int fd = open(path, O_RDONLY);
    FAIL_IF(fd < 0, "Failed to open file '%s': %s (%d)", path, strerror(errno), errno);
    /* Query the filesystem about the file's size */
    struct stat stat;
    FAIL_IF(fstat(fd, &stat), "fstat failed for file '%s' with error: %s (%d)", path, strerror(errno), errno);
    /* Check that the file is at least as large as we need it to be. Note that the case when the parameter size=0
     * is handled gracefully here. */
    FAIL_IF(stat.st_size < size, "File '%s' is smaller (%d) than expected (%d)", path, stat.st_size, size);
    if (size == 0) {
        /* Map the entire file */
        size = stat.st_size;
    }
    /* Map the file into the process' address space */
    void *mapping = mmap(NULL, size, PROT_READ, MAP_PRIVATE | MAP_POPULATE | MAP_LOCKED, fd, 0);
    FAIL_IF(mapping == MAP_FAILED, "mmap failed to map file '%s' with error: %s (%d)", path, strerror(errno), errno);
    /* Close the descriptor */
    (void) close(fd);

    return mapping;
}

static size_t get_filesize(const char *path)
{
    /* Open the file for reading */
    int fd = open(path, O_RDONLY);
    FAIL_IF(fd < 0, "Failed to open file '%s': %s (%d)", path, strerror(errno), errno);
    /* Query the filesystem about the file's size */
    struct stat stat;
    FAIL_IF(fstat(fd, &stat), "fstat failed for file '%s' with error: %s (%d)", path, strerror(errno), errno);
    /* Close the descriptor */
    (void) close(fd);
    return stat.st_size;
}
