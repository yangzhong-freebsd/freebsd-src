local cfg = {}

-- what gets passed into bsdinstall scriptedpart
cfg.partitions = "ada0"

cfg.distributions = "kernel.txz base.txz src.txz"

cfg.users = {
	{
		username = "name",
		full_name = "Name O. Person",
		shell = "/bin/sh",
		password = "thepassword",
	},
}

cfg.rootpass = "rootpassword"

cfg.hostname = "test"

return cfg
