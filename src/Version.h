#define VER_MAJOR      0
#define VER_MINOR      1
#define VER_PATCH      0  
#define VER_REVISION   0

#define RELEASE_DATE           "@RELEASE_DATE@"

#define APR_STRINGIFY(n) APR_STRINGIFY_HELPER(n)
#define APR_STRINGIFY_HELPER(n) #n

/** Version number */
#define VER_NUM                APR_STRINGIFY(VER_MAJOR) \
                           "." APR_STRINGIFY(VER_MINOR) \
                           "." APR_STRINGIFY(VER_PATCH) \
                           "." APR_STRINGIFY(VER_REVISION)



#define PRODUCT_NAME       "Phast for Windows" \
                       "-" APR_STRINGIFY(VER_MAJOR) \
                       "." APR_STRINGIFY(VER_MINOR) \
                       "." APR_STRINGIFY(VER_REVISION)


#define VER_NUM_REG                 APR_STRINGIFY(VER_MAJOR) \
                                "." APR_STRINGIFY(VER_MINOR)

#define REVISION               APR_STRINGIFY(VER_REVISION)
