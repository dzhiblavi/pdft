local home = os.getenv("HOME")
local c = require("config.default")
local p = c.plugins

c.editor.indent = 4
c.editor.column_limit = 100
c.features.arc_support = false

p.debug.cc = {
    default_adapter = "codelldb",
    adapters = {
        codelldb = {
            command = home .. "/.local/share/nvim/mason/bin/codelldb",
        }
    }
}

p.lsp.none_ls.format_on_save = false

p.lsp.nvim_lspconfig.servers.clangd.cmd = {
    "clangd",
    "--background-index=true",
    "--clang-tidy=true",
    "--all-scopes-completion",
    "--offset-encoding=utf-16",
    "--completion-style=detailed",
    "--pch-storage=memory",
    "--header-insertion=never",
    "--log=error",
    "--pretty",
    "-j=8",
    "--compile-commands-dir=/Users/dzhiblavi/personal/projects/pdft-cxx/target/Debug/build",
}

return c
