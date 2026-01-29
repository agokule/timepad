vim.o.autochdir = false

vim.lsp.enable('clangd')

vim.o.makeprg = 'cmake --build build'
local compile = { 'cmake', '--build', 'build' }

local function shallowCopy(orig)
    local copy = {}
    for k, v in pairs(orig) do
        copy[k] = v
    end
    return copy
end

local configure = { 'cmake', '-S', '.', '-B', 'build' }
local config_rel = shallowCopy(configure)
table.insert(config_rel, '-DCMAKE_BUILD_TYPE=Release')

local config_dbg = shallowCopy(configure)
table.insert(config_dbg, '-DCMAKE_BUILD_TYPE=Debug')

-- got these 2 functions from https://github.com/sakhnik/make-async.nvim/blob/2846b31992dd49a741f18cef21f99a5c52cc3d3c/lua/make-async.lua
local function qf_clear()
  vim.fn.setqflist({}, "r")
end
local function qf_append(title, lines)
  vim.fn.setqflist({}, "a", { title = title, lines = lines, })
end

-- Function to split a string into lines
local function split_lines(str)
    local lines = {}
    if type(str) ~= "string" then
        error("Input must be a string")
    end

    -- Normalize all line endings to \n
    str = str:gsub("\r\n", "\n"):gsub("\r", "\n")

    -- Iterate over each line, skipping the trailing empty match
    for line in str:gmatch("[^\n]+") do
        table.insert(lines, line)
    end
    return lines
end

-- Function to extend one table with another
local function extend(t1, t2)
    -- Validate inputs
    if type(t1) ~= "table" or type(t2) ~= "table" then
        error("Both arguments must be tables (arrays).")
    end
    
    for i = 1, #t2 do
        table.insert(t1, t2[i])
    end
end

local cmake_efm = table.concat({
    -- Prepend noise filters for CMake progress and GCC code snippets
    "%-G[%\\s%#%\\d%\\+%%]%[ ]%#Built target %m",
    "%-G[%\\s%#%\\d%\\+%%]%[ ]%#Building %m",
    "%-Gmake%[%^:]%#:%m",
    "%-G%[ ]%#%\\d%# | %m",
    "%-G%[ ]%#| %m",
    "%f:%l:%c: %t%*[^:]: %m",
    "%f:%l:%c: %m",
    "%f:%l: %t%*[^:]: %m",
    "%-G%f:%l: In function %m:",
    "%-G%f: In function %m:",
    "%-G%\\s%#", -- Ignore empty or whitespace-only lines

    -- Original exhaustive patterns and directory tracking
    "%*[^\"]\"%f\"%*\\D%l: %m",
    "\"%f\"%*\\D%l: %m",
    "%-Gg%\\?make[%*\\d]: *** [%f:%l:%m",
    "%-Gg%\\?make: *** [%f:%l:%m",
    "%-G%f:%l: (Each undeclared identifier is reported only once,",
    "%-G%f:%l: for each function it appears in.)",
    "%-GIn file included from %f:%l:%c:",
    "%-GIn file included from %f:%l:%c\\,",
    "%-GIn file included from %f:%l:%c",
    "%-GIn file included from %f:%l",
    "%-G%*[ ]from %f:%l:%c",
    "%-G%*[ ]from %f:%l:",
    "%-G%*[ ]from %f:%l\\,",
    "%-G%*[ ]from %f:%l",
    "%f:%l:%c:%m",
    "%f(%l):%m",
    "%f:%l:%m",
    "\"%f\"\\, line %l%*\\D%c%*[^ ] %m",
    "%D%*\\a[%*\\d]: Entering directory %*[`']%f'",
    "%X%*\\a[%*\\d]: Leaving directory %*[`']%f'",
    "%D%*\\a: Entering directory %*[`']%f'",
    "%X%*\\a: Leaving directory %*[`']%f'",
    "%DMaking %*\\a in %f",
    "%f|%l| %m",
}, ",")
vim.o.errorformat = cmake_efm

local spinner = { "⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏" }

local function notify_progress(id, title, newline)
    vim.notify(newline, "info", {
        id = id,
        title = title,
        opts = function(notif)
            if string.match(newline, "Build files have been written to:") or string.match(newline, "Built target") then
                notif.icon = " "
            else
                notif.icon = spinner[math.floor(vim.uv.hrtime() / (1e6 * 80)) % #spinner + 1]
            end
        end
    })
end

local function notify_error(title, newline)
    vim.notify(newline, "error", { title = title .. " Error" })
end

local function run_task(cmd, title, id, next_task)
    qf_clear()
    local output = {}

    vim.system(cmd, {
        text = true,
        stderr = function(_, newline)
            if not newline then return end
            local lines = split_lines(newline)
            extend(output, lines)
            notify_error(title, newline)
        end,
        stdout = function(_, newline)
            if not newline then return end
            local lines = split_lines(newline)
            extend(output, lines)
            notify_progress(id, title, newline)
        end,
    }, function(obj)
        if obj.code ~= 0 then
            vim.schedule(function()
                vim.notify(title .. " failed with code " .. obj.code, "error")
                qf_append(title, output)
            end)
            return
        end
        if next_task then
            vim.schedule(next_task)
        end
    end)
end

local function config(conf)
    run_task(conf, "CMake Configure", "cmake_configure", function()
        run_task(compile, "CMake Build", "cmake_build")
    end)
end

vim.keymap.set('n', '<leader>rb', function() config(config_dbg) end)
vim.keymap.set('n', '<leader>rB', function() config(config_rel) end)
vim.keymap.set('n', '<leader>rp', function() vim.cmd('botright sp') vim.cmd.terminal('./build/Timepad') end)

