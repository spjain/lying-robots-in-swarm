set(BUZZ_BZZ_INCLUDE_DIR "${BUZZ_BZZ_INCLUDE_DIR}:${CMAKE_CURRENT_SOURCE_DIR}")
# buzz_make(Algo1_basic_pattern_recognition.bzz)
# buzz_make(Algo2_average_belief.bzz)
# buzz_make(Algo2_average_belief_limited_comm.bzz)
# buzz_make(Algo3_SSR.bzz)
# buzz_make(Algo3_SSR_limited_comm.bzz)
buzz_make(main.bzz INCLUDES belief.bzz bad_fun.bzz good_fun.bzz motion.bzz)
