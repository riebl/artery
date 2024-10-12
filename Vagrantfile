Vagrant.configure("2") do |config|
    config.vm.box = "generic/debian12"
    config.vm.synced_folder ".", "/vagrant"
    config.vm.provision "ansible_local" do |ansible|
      ansible.playbook = "ansible/vagrant.yml"
    end

    config.vm.provider :virtualbox do |vb|
        # distinguish VMs by a location-dependent suffix
        name_suffix = Digest::SHA1.hexdigest(Dir.pwd)[0..6]
        vb.name = "Artery Vagrant " + name_suffix
        vb.gui = true
        vb.memory = 4096
        vb.cpus = 4
        vb.customize ["modifyvm", :id, "--graphicscontroller", "vmsvga"]
        vb.customize ["modifyvm", :id, "--vram", "64"]
        vb.customize ['modifyvm', :id, "--accelerate3d", "on"]
    end
end
