def vbox_name(variant="")
  name = "Artery Vagrant VM"
  if not variant.empty? then
    name += " (#{variant})"
  end

  # distinguish VMs across filesystem by a location-dependent suffix
  suffix = Digest::SHA1.hexdigest(Dir.pwd)[0..6]

  return "#{name} #{suffix}"
end

Vagrant.configure("2") do |config|
    config.vm.box = "debian/contrib-buster64"

    config.vm.provision "ansible_local" do |ansible|
      ansible.compatibility_mode = "2.0"
      ansible.playbook = "ansible/vagrant.yml"
      ansible.groups = {
        "ros_machines" => [:ros]
      }
    end

    config.vm.define :basic, primary: true
    config.vm.define :ros, autostart: false do |ros|
      ros.vm.box = "hashicorp/bionic64"
      ros.vm.provider :virtualbox do |vb|
        vb.name = vbox_name("ROS")
      end
    end

    config.vm.provider :virtualbox do |vb|
      vb.name = vbox_name()
      vb.gui = true
      vb.memory = 2048
      vb.customize ["modifyvm", :id, "--vram", "32"]
    end
end
