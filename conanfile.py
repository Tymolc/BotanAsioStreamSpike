from conans import ConanFile
from conans.tools import os_info

class SpikeBeast(ConanFile):
    name = 'Spike Beast'
    description = ''
    generators = 'cmake'
    settings = 'os', 'compiler', 'build_type', 'arch'

    def requirements(self):
        self.requires("botan/2.12.0@nexenio/testing")
        self.requires("boost/1.69.0@conan/stable")
        self.requires("cpprestsdk/2.10.14.16@nexenio/stable")

    def _botan_modules12(self):
        modules = [
            "asio", "auto_rng", "cbc", "cbc_mac", "ccm", "certstor_flatfile",
            "certstor_system", "certstor_system_macos",
            "certstor_system_windows", "chacha20poly1305", "des", "dev_random",
            "dsa", "eme_oaep", "getentropy", "hkdf", "locking_allocator",
            "nist_keywrap", "pbes2", "pbkdf2", "rfc6979", "system_rng", "sha3",
            "tls", "win32_stats", "pkcs11"
        ]

        if self.settings.arch == "x86_64":
            modules += [
                "aes_ni", "chacha_avx2", "chacha_simd32", "clmul_cpu",
                "clmul_ssse3", "rdrand", "rdseed", "sha1_sse2", "sha1_x86",
                "sha2_64_bmi2", "sha3_bmi2", "simd", "simd_avx2"
            ]

        modules.sort()  # makes sure that reordering modules does not trigger an
                        # unnecessary rebuild of the botan dependency
        return ','.join(modules)

    def _botan_modules11(self):
        modules = [
            "asio", "auto_rng", "cbc", "cbc_mac", "ccm", "certstor_flatfile",
            "certstor_system", "certstor_system_macos",
            "certstor_system_windows", "chacha20poly1305", "des", "dev_random",
            "dsa", "eme_oaep", "getentropy", "hkdf", "locking_allocator",
            "nist_keywrap", "pbes2", "pbkdf2", "rfc6979", "system_rng", "sha3",
            "tls", "win32_stats", "pkcs11",
        ]

        if self.settings.arch == "x86_64":
            modules += [
                "aes_ni", "aes_ssse3", "chacha_avx2", "chacha_simd32", "clmul",
                "clmul_ssse3", "rdrand", "rdseed", "sha1_sse2", "sha1_x86",
                "sha2_64_bmi2", "sha3_bmi2", "simd", "simd_avx2"
            ]

        modules.sort()  # makes sure that reordering modules does not trigger an
                        # unnecessary rebuild of the botan dependency
        return ','.join(modules)

    def configure(self):
        self.options["cpprestsdk"].add("exclude_websockets=True")

        self.options["botan"].add("shared=False")
        self.options["botan"].add("boost=True")
        self.options["botan"].add("enable_modules=%s" % self._botan_modules12())
        self.options["botan"].add("quiet=False")

        self.options["boost"].add("shared=False")
        self.options["boost"].add("without_math=True")
        self.options["boost"].add("without_wave=True")
        self.options["boost"].add("without_container=True")
        self.options["boost"].add("without_contract=True")
        self.options["boost"].add("without_exception=True")
        self.options["boost"].add("without_graph=True")
        self.options["boost"].add("without_iostreams=True")
        self.options["boost"].add("without_locale=True")
        self.options["boost"].add("without_log=True")
        self.options["boost"].add("without_program_options=True")
        self.options["boost"].add("without_mpi=True")
        self.options["boost"].add("without_serialization=True")
        self.options["boost"].add("without_fiber=True")
        self.options["boost"].add("without_context=True")
        self.options["boost"].add("without_timer=True")
        self.options["boost"].add("without_graph_parallel=True")
        self.options["boost"].add("without_stacktrace=True")
        self.options["boost"].add("without_test=True")
        self.options["boost"].add("without_type_erasure=True")
        self.options["boost"].add("without_python=True")
